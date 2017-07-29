//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "Geomlib_TriMeshEdgeSplit.h"

#include <iostream>
#include <vector>

#include <Eigen/Core>

#pragma warning(push, 0)
#include <igl/edge_flaps.h>
#include <igl/is_edge_manifold.h>
#include <igl/is_vertex_manifold.h>
#include <igl/is_border_vertex.h>
#pragma warning(pop)

#include <Urho3D/IO/Log.h>

#include "TriMesh.h"
#include "ConversionUtilities.h"

#pragma warning(disable : 4244)

using Urho3D::Variant;

namespace {

/*==============================================================================
ASSUMPTIONS:
Original mesh was OV, OF, OE, EMAP.
Looped over edges in OE, and edges to be split were marked.
Sizes of new vertex and face matrices were determined, and
V, F, E were initialized to the right size, matching OV, OF, OE in
their starting blocks.

e:       index into OE of original edge to split
i:       e is the ith edge to be split (starting at 0)
V, F, E: partially updated matrices before edge e is split

Outputs:
V, F, E: partially updated matrices after edge e is split
==============================================================================*/
void split_edge(
	int e,
	int count,
	int orig_vcount,
	int orig_fcount,
	int orig_ecount,
	Eigen::MatrixXf& V,
	Eigen::MatrixXi& F,
	Eigen::MatrixXi& E,
	Eigen::MatrixXi& EMAP,
	Eigen::MatrixXi& EF,
	Eigen::MatrixXi& EI
)
{
	int i = E(e, 0);
	int j = E(e, 1);

	int f0 = EF(e, 0); // face opposite e=(i-->j)
	int f1 = EF(e, 1); // face opposite e=(j-->i)

	int k = F(f0, EI(e, 0));
	int l = F(f1, EI(e, 1));

	int r = orig_vcount + count;
	V.row(r) = 0.5f * (V.row(i) + V.row(j));

	int f = orig_fcount + 2 * count;
	// update row f0
	int v = EI(e, 0);
	int ejtok = EMAP(f0, (v + 1) % 3);
	F(f0, (v + 2) % 3) = r;
	// update row f1
	int w = EI(e, 1);
	int eltoj = EMAP(f1, (w + 2) % 3);

	F(f1, (w + 1) % 3) = r;
	// fill row f
	F.row(f) = Eigen::RowVector3i(j, k, r);
	// fill row f + 1
	F.row(f + 1) = Eigen::RowVector3i(l, j, r);

	int s = orig_ecount + 3 * count;
	// update row e from e=(i-->j) to e=(i-->r)
	E(e, 1) = r;
	// fill row s with (r-->j)
	E.row(s) = Eigen::RowVector2i(r, j);
	// fill row s + 1
	E.row(s + 1) = Eigen::RowVector2i(k, r);
	// fill row s + 2
	E.row(s + 2) = Eigen::RowVector2i(r, l);

	// EF
	// EF.row(e) does not require update
	// s=(r-->j)
	EF.row(s) = Eigen::RowVector2i(f + 1, f);
	EF.row(s + 1) = Eigen::RowVector2i(f, f0);
	EF.row(s + 2) = Eigen::RowVector2i(f + 1, f1);

	if (EF(ejtok, 0) == f0) {
		EF(ejtok, 0) = f;
		EI(ejtok, 0) = 2;
	}
	else {
		EF(ejtok, 1) = f;
		EI(ejtok, 1) = 2;
	}
	if (EF(eltoj, 0) == f1) {
		EF(eltoj, 0) = f + 1;
		EI(eltoj, 0) = 2;
	}
	else {
		EF(eltoj, 1) = f + 1;
		EI(eltoj, 1) = 2;
	}

	// s used for EI
	// EI.row(e) does not require update
	EI.row(s) = Eigen::RowVector2i(1, 0); // refers to lines 65, 67 respectively
	EI.row(s + 1) = Eigen::RowVector2i(0, (v + 1) % 3);
	EI.row(s + 2) = Eigen::RowVector2i(1, (w + 2) % 3);

	// where it used to say j->k should now be r->k i.e. edge s+1
	EMAP(f0, (v + 1) % 3) = s + 1;
	// where it used to say l->j should now be l->r i.e. edge s+2
	EMAP(f1, (w + 2) % 3) = s + 2;
	// ? is j-->k
	EMAP.row(f) = Eigen::RowVector3i(s + 1, s, ejtok);
	// ? is l-->j
	EMAP.row(f + 1) = Eigen::RowVector3i(s, s + 2, eltoj);
}

void split_edges_above_length(
	const Eigen::MatrixXf& OV,
	const Eigen::MatrixXi& OF,
	Eigen::MatrixXf& NV,
	Eigen::MatrixXi& NF,
	float L
)
{
	Eigen::MatrixXi OE, EF, EI;
	Eigen::VectorXi EMAP;

	igl::edge_flaps(OF, OE, EMAP, EF, EI);
	Eigen::MatrixXi NE = OE;

	NV = OV;
	NF = OF;

	/***********************/

	std::vector<bool> border_verts = igl::is_border_vertex(NV, NF);

	/***********************/

	std::vector<int> edges_to_split;

	for (int e = 0; e < OE.rows(); ++e) {

		bool totally_interior_edge = false;
		if (
			border_verts[OE(e, 0)] == false &&
			border_verts[OE(e, 1)] == false
			)
		{
			totally_interior_edge = true;
		}

		// after we see if this is faster, convert to SQUAREDNORM!
		float len = (OV.row(OE(e, 0)) - OV.row(OE(e, 1))).norm();
		if (len > L && totally_interior_edge) {
			edges_to_split.push_back(e);
		}
	}

	int split_count = edges_to_split.size();

	/***********************/

	int orig_vcount = NV.rows();
	int orig_fcount = NF.rows();
	int orig_ecount = NE.rows();

	NV.conservativeResize(NV.rows() + split_count, Eigen::NoChange);
	NF.conservativeResize(NF.rows() + 2 * split_count, Eigen::NoChange);
	NE.conservativeResize(NE.rows() + 3 * split_count, Eigen::NoChange);
	EF.conservativeResize(EF.rows() + 3 * split_count, Eigen::NoChange);
	EI.conservativeResize(EI.rows() + 3 * split_count, Eigen::NoChange);

	Eigen::MatrixXi NEMAP(NF.rows(), 3);
	NEMAP.block(0, 0, OF.rows(), 1) = EMAP.block(0, 0, OF.rows(), 1);
	NEMAP.block(0, 1, OF.rows(), 1) = EMAP.block(OF.rows(), 0, OF.rows(), 1);
	NEMAP.block(0, 2, OF.rows(), 1) = EMAP.block(2 * OF.rows(), 0, OF.rows(), 1);

	for (int count = 0; count < edges_to_split.size(); ++count) {
		int e = edges_to_split[count];
		split_edge(
			e,
			count,
			orig_vcount,
			orig_fcount,
			orig_ecount,
			NV,
			NF,
			NE,
			NEMAP,
			EF,
			EI
		);
	}
}

} // namespace

Urho3D::Variant Geomlib::TriMesh_SplitLongEdges(
	const Urho3D::Variant& tri_mesh,
	float split_threshold
)
{
	if (!TriMesh_Verify(tri_mesh)) {
		return Variant();
	}

	Eigen::MatrixXf V, NV;
	Eigen::MatrixXi F, NF;
	bool igl_success = IglMeshToMatrices(tri_mesh, V, F);
	if (!igl_success) {
		return Variant();
	}

	if (!igl::is_edge_manifold(F)) {
		return Variant();
	}

	Eigen::VectorXi B;
	if (!igl::is_vertex_manifold(F, B)) {
		URHO3D_LOGINFO("TriMeshEdgeSplit --- not a vertex manifold, exiting");
		return Variant();
	}

	split_edges_above_length(V, F, NV, NF, split_threshold);

	return TriMesh_Make(NV, NF);
}