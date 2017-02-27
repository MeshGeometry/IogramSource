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


#include "Geomlib_TriMeshSubdivide.h"

#include <iostream>
#include <vector>

#include <Urho3D/Core/Variant.h>

#include <Eigen/Core>
#include <Eigen/Dense>

#pragma warning(push, 0)
#include <igl/edges.h>
#include <igl/edge_flaps.h>
#include <igl/unique_edge_map.h>
#pragma warning(pop)

#include "ConversionUtilities.h"
#include "TriMesh.h"

#pragma warning(disable : 4244)

// collection of helper functions quarantined in anonymous namespace
namespace {

	// reviewed: CHECK
	int look_up_edge_index(const Eigen::MatrixXi& E, int s, int t)
	{
		int edge_index = -1;
		for (unsigned e = 0; e < E.rows(); ++e) {
			if ((E(e, 0) == s && E(e, 1) == t) || (E(e, 0) == t && E(e, 1) == s)) {
				edge_index = e;
				break;
			}
		}
		return edge_index;
	}


	void face_edge_map(
		const Eigen::MatrixXi& F,
		const Eigen::MatrixXi& E,
		Eigen::MatrixXi& FE)
	{
		FE.setZero(F.rows(), 3);

		Eigen::VectorXi EMAP;
		Eigen::MatrixXi NE, EF, EI;
		igl::edge_flaps(F, NE, EMAP, EF, EI);

		for (int f = 0; f < F.rows(); ++f) {
			for (int i = 0; i < 3; ++i) {
				int j = (i + 1) % 3;
				int k = (i + 2) % 3;
				int r, s, t;
				r = F(f, i);
				s = F(f, j);
				t = F(f, k);
				int e = look_up_edge_index(E, s, t);
				FE(f, i) = e;
			}
		}
		return;
	}

	// If a row of F has entries
	//   a, b, c
	// Then the corresponding row for FM has entries
	//   i, j, k
	// where
	//   i is the index (into edge-subdivided vertex matrix) of the midpoint of the edge joining (b,c)
	//   j is the index (into edge-subdivided vertex matrix) of the midpoint of the edge joining (c,a)
	//   k is the index (into edge-subdivided vertex matrix) of the midpoint of the edge joining (a,b)
	void face_edge_midpoint_map(
		int numVertices,
		const Eigen::MatrixXi& F,
		const Eigen::MatrixXi& E,
		Eigen::MatrixXi& FM)
	{
		FM.setZero(F.rows(), 3);

		for (int f = 0; f < F.rows(); ++f) {
			for (int i = 0; i < 3; ++i) {
				int j = (i + 1) % 3;
				int k = (i + 2) % 3;
				int r, s, t;
				r = F(f, i);
				s = F(f, j);
				t = F(f, k);
				int e = look_up_edge_index(E, s, t);
				FM(f, i) = e + numVertices;
			}
		}
		return;
	}

	void subdivide_mesh(
		const Eigen::MatrixXd& OV,
		const Eigen::MatrixXi& OF,
		Eigen::MatrixXd& NV,
		Eigen::MatrixXi& NF)
	{
		Eigen::MatrixXi E;
		Eigen::MatrixXi uE;
		Eigen::VectorXi EMAP;
		std::vector<std::vector<int> > uE2E;
		igl::unique_edge_map(OF, E, uE, EMAP, uE2E);

		// add midpoints to edge-subdivided vertex matrix NV
		NV.setZero(OV.rows() + uE.rows(), 3);
		NV.block(0, 0, OV.rows(), 3) = OV;
		for (int e = 0; e < uE.rows(); ++e) {
			int v, w;
			v = uE(e, 0);
			w = uE(e, 1);
			NV.row(OV.rows() + e) = 0.5 * (OV.row(v) + OV.row(w));
		}

		// compute subdivided face data into NF
		NF.setZero(4 * OF.rows(), 3);
		for (int f = 0; f < OF.rows(); ++f) {
			int v0, v1, v2;
			v0 = OV.rows() + EMAP(f);
			v1 = OV.rows() + EMAP(OF.rows() + f);
			v2 = OV.rows() + EMAP(2 * OF.rows() + f);
			//
			NF.row(4 * f) = Eigen::RowVector3i(v0, v1, v2);
			NF.row(4 * f + 1) = Eigen::RowVector3i(OF(f, 0), v2, v1);
			NF.row(4 * f + 2) = Eigen::RowVector3i(v2, OF(f, 1), v0);
			NF.row(4 * f + 3) = Eigen::RowVector3i(v1, v0, OF(f, 2));
		}
		return;
	}

	void subdivide_mesh(
		const Eigen::MatrixXd& OV,
		const Eigen::MatrixXi& OF,
		const Eigen::VectorXi& KEEP,
		Eigen::MatrixXd& NV,
		Eigen::MatrixXi& NF,
		Eigen::VectorXi& NKEEP)
	{
		Eigen::MatrixXi E;
		Eigen::MatrixXi uE;
		Eigen::VectorXi EMAP;
		std::vector<std::vector<int> > uE2E;
		igl::unique_edge_map(OF, E, uE, EMAP, uE2E);

		NKEEP.setZero(4 * KEEP.rows());

		// add midpoints to edge-subdivided vertex matrix NV
		NV.setZero(OV.rows() + uE.rows(), 3);
		NV.block(0, 0, OV.rows(), 3) = OV;
		for (int e = 0; e < uE.rows(); ++e) {
			int v, w;
			v = uE(e, 0);
			w = uE(e, 1);
			NV.row(OV.rows() + e) = 0.5 * (OV.row(v) + OV.row(w));
		}

		// compute subdivided face data into NF
		NF.setZero(4 * OF.rows(), 3);
		for (int f = 0; f < OF.rows(); ++f) {
			int v0, v1, v2;
			v0 = OV.rows() + EMAP(f);
			v1 = OV.rows() + EMAP(OF.rows() + f);
			v2 = OV.rows() + EMAP(2 * OF.rows() + f);
			//
			NF.row(4 * f) = Eigen::RowVector3i(v0, v1, v2);
			NF.row(4 * f + 1) = Eigen::RowVector3i(OF(f, 0), v2, v1);
			NF.row(4 * f + 2) = Eigen::RowVector3i(v2, OF(f, 1), v0);
			NF.row(4 * f + 3) = Eigen::RowVector3i(v1, v0, OF(f, 2));
			int val = KEEP(f);
			NKEEP(4 * f) = val;
			NKEEP(4 * f + 1) = val;
			NKEEP(4 * f + 2) = val;
			NKEEP(4 * f + 3) = val;
		}
		return;
	}

	void double_subdivide_mesh(
		const Eigen::MatrixXd& OV,
		const Eigen::MatrixXi& OF,
		Eigen::MatrixXd& NV,
		Eigen::MatrixXi& NF)
	{
		Eigen::MatrixXd V;
		Eigen::MatrixXi F;
		subdivide_mesh(OV, OF, V, F);
		subdivide_mesh(V, F, NV, NF);

		return;
	}

	void subdivide_mesh(
		const Eigen::MatrixXd& OV,
		const Eigen::MatrixXi& OF,
		const Eigen::VectorXi& KEEP,
		Eigen::MatrixXd& NV,
		Eigen::MatrixXi& NF,
		Eigen::VectorXi& NKEEP,
		int steps)
	{
		Eigen::MatrixXd V = OV;
		Eigen::MatrixXi F = OF;
		Eigen::VectorXi nKEEP = KEEP;
		for (int i = 0; i < steps; ++i) {
			subdivide_mesh(V, F, nKEEP, NV, NF, NKEEP);
			V.setZero(NV.rows(), 3);
			F.setZero(NF.rows(), 3);
			nKEEP.setZero(NKEEP.rows());
			V = NV;
			F = NF;
			nKEEP = NKEEP;
		}
		return;
	}



	///////////////////////////////////////////////////////////////////////////////////////////////

	// Inputs:
	//     V: #V by 3 list of vertices
	//     F: #F by 3 list of faces into V
	//     KEEP: #F by 1 list taking values -1 (in shadow), 0 (on shadow-light boundary), 1 (in sunlight)
	// Outputs:
	//     NV: #NV by 3 list of vertices in the subdivided mesh
	//     NF: #NF by 3 list of faces (into NV) of the subdivided mesh
	//     NKEEP: #NF by 1 list taking values -1, 0, 1
	void subdivide_step(
		const Eigen::MatrixXd& V,
		const Eigen::MatrixXi& F,
		const Eigen::VectorXi& KEEP,
		Eigen::MatrixXd& NV,
		Eigen::MatrixXi& NF,
		Eigen::VectorXi& NKEEP)
	{
		Eigen::VectorXi nKEEP = KEEP;

		// compute adjacency data
		Eigen::MatrixXi E;
		Eigen::MatrixXi uE;
		Eigen::VectorXi EMAP;
		std::vector<std::vector<int> > uE2E;
		igl::unique_edge_map(F, E, uE, EMAP, uE2E);

		// set up the new vertex matrix
		// make it as large as it could possibly need to be, i.e., in case every face gets subdivided
		NV.setZero(V.rows() + uE.rows(), 3);
		NV.block(0, 0, V.rows(), 3) = V;
		// set up the new face matrix
		// make it as large as it could possibly need to be, i.e., in case every face gets subdivided
		NF.resize(4 * F.rows(), 3);
		NF = Eigen::MatrixXi::Constant(4 * F.rows(), 3, -1);
		NF.block(0, 0, F.rows(), 3) = F;
		NKEEP.setZero(NF.rows());


		// FIRST PASS of FACE SUBDIVISION

		// Rows of SPLIT correspond to rows of uE (unordered edges).
		// If row e of SPLIT has value -1 then row e (index into uE) has not yet been split.
		// If row e of SPLIT has non-negative value i then edge e has been split,
		// in the sense that its midpoint has been added to the updated vertex matrix
		// and its index there is i.
		Eigen::VectorXi SPLIT = Eigen::VectorXi::Constant(uE.rows(), -1);

		int facesDivided = 0;
		int edgesSplit = 0;
		std::vector<int> undivided_faces;
		for (int f = 0; f < F.rows(); ++f) {
			// indices into V (and therefore into NV) of vertices of face f
			int i, j, k;
			i = F(f, 0);
			j = F(f, 1);
			k = F(f, 2);
			// indices into uE of edges of face f
			int e0, e1, e2;
			e0 = EMAP(f);
			e1 = EMAP(F.rows() + f);
			e2 = EMAP(2 * F.rows() + f);
			// indices into NV of midpoints of edges e0, e1, e2
			int m0, m1, m2;

			if (nKEEP(f) == 0) {
				// split edges with indices e0, e1, e2 if they have not already been split

				// split e0 if necessary
				int v = SPLIT(e0);
				m0 = v;
				if (v == -1) {
					v = V.rows() + edgesSplit;
					SPLIT(e0) = v;
					m0 = v;
					// do the split
					NV.row(v) = 0.5 * (V.row(uE(e0, 0)) + V.row(uE(e0, 1)));
					edgesSplit++;
				}
				// split e1 if necessary
				v = SPLIT(e1);
				m1 = v;
				if (v == -1) {
					v = V.rows() + edgesSplit;
					SPLIT(e1) = v;
					m1 = v;
					// do the split
					NV.row(v) = 0.5 * (V.row(uE(e1, 0)) + V.row(uE(e1, 1)));
					edgesSplit++;
				}
				// split e2 if necessary
				v = SPLIT(e2);
				m2 = v;
				if (v == -1) {
					v = V.rows() + edgesSplit;
					SPLIT(e2) = v;
					m2 = v;
					// do the split
					NV.row(v) = 0.5 * (V.row(uE(e2, 0)) + V.row(uE(e2, 1)));
					edgesSplit++;
				}

				// Now all the edges are split (whether we did it just now or it was split at a previous face).
				// face f has vertices with indices i,j,k into NV
				// midpoints of edges have indices m0,m1,m2 into NV
				// and the four faces should go into NF with indices
				//   f (inner face overwrites big original face)
				//   f0, f1, f2
				int f0, f1, f2;
				f0 = F.rows() + 3 * facesDivided;
				f1 = f0 + 1;
				f2 = f0 + 2;
				facesDivided++;
				NF.row(f) = Eigen::RowVector3i(m0, m1, m2);
				NF.row(f0) = Eigen::RowVector3i(i, m2, m1);
				NF.row(f1) = Eigen::RowVector3i(m2, j, m0);
				NF.row(f2) = Eigen::RowVector3i(m1, m0, k);
			}
			else {
				undivided_faces.push_back(f);
			}
		}

		// clean up new vertex matrix NV
		NV.conservativeResize(V.rows() + edgesSplit, 3);

		// SECOND PASS of FACE SUBDIVISION, i.e., clean up

		for (int i = 0; i < undivided_faces.size(); ++i) {
			int f = undivided_faces[i];
			int e0, e1, e2;
			e0 = EMAP(f);
			e1 = EMAP(f + F.rows());
			e2 = EMAP(f + 2 * F.rows());
			int m0, m1, m2;
			m0 = SPLIT(e0);
			m1 = SPLIT(e1);
			m2 = SPLIT(e2);
			int v0, v1, v2;
			v0 = F(f, 0);
			v1 = F(f, 1);
			v2 = F(f, 2);
			int f0, f1, f2;
			f0 = F.rows() + 3 * facesDivided;
			f1 = f0 + 1;
			f2 = f0 + 2;
			facesDivided++;

			int l = nKEEP(f);
			NKEEP(f) = l;
			NKEEP(f0) = l;
			NKEEP(f1) = l;
			NKEEP(f2) = l;

			if (m0 == m1 && m1 == m2) {
				// face has had NO edges split
			}
			else if (m0 == m1) {
				// face has had only edge e2 split
				NF.row(f) = Eigen::RowVector3i(v0, m2, v2);
				NF.row(f0) = Eigen::RowVector3i(m2, v1, v2);
			}
			else if (m1 == m2) { // -1, -1, m0
								 // face has had only edge e0 split
				NF.row(f) = Eigen::RowVector3i(v1, m0, v0);
				NF.row(f0) = Eigen::RowVector3i(m0, v2, v0);
			}
			else if (m2 == m0) { // -1, -1, m1
								 // face has had only edge e1 split
				NF.row(f) = Eigen::RowVector3i(v2, m1, v1);
				NF.row(f0) = Eigen::RowVector3i(m1, v0, v1);
			}
			else {
				if (m0 == -1) {
					// face has had edges e1, e2 split
					NF.row(f) = Eigen::RowVector3i(v1, v2, m2);
					NF.row(f0) = Eigen::RowVector3i(m2, v2, m1);
					NF.row(f1) = Eigen::RowVector3i(m2, m1, v0);
				}
				else if (m1 == -1) {
					// face has had edges e0, e2 split
					NF.row(f) = Eigen::RowVector3i(v2, v0, m0);
					NF.row(f0) = Eigen::RowVector3i(m0, v0, m2);
					NF.row(f1) = Eigen::RowVector3i(m0, m2, v1);
				}
				else if (m2 == -1) {
					// face has had edges e0, e1 split
					NF.row(f) = Eigen::RowVector3i(v0, v1, m1);
					NF.row(f0) = Eigen::RowVector3i(m1, v1, m0);
					NF.row(f1) = Eigen::RowVector3i(m1, m0, v2);
				}
				else {
					// all three edges e0, e1, e2 have been split
					// face f has vertices with indices v0, v1, v2 into NV
					// midpoints of edges have indices m0, m1, m2 into NV
					// and the four faces should go into NF with indices
					//   f (inner face overwrites big original face)
					//   f0, f1, f2
					NF.row(f) = Eigen::RowVector3i(m0, m1, m2);
					NF.row(f0) = Eigen::RowVector3i(v0, m2, m1);
					NF.row(f1) = Eigen::RowVector3i(m2, v1, m0);
					NF.row(f2) = Eigen::RowVector3i(m1, m0, v2);
				}
			}

		}

		// clean up new face matrix NF and new kept faces vector NKEEP
		std::vector<int> faces;
		std::vector<int> keep;
		for (int f = 0; f < NF.rows(); ++f) {
			if (NF(f, 0) != -1) {
				faces.push_back(NF(f, 0));
				faces.push_back(NF(f, 1));
				faces.push_back(NF(f, 2));
				keep.push_back(NKEEP(f));
			}
		}
		int numFaces = faces.size() / 3;
		NF.resize(numFaces, 3);
		NKEEP.resize(numFaces);
		for (int f = 0; f < NF.rows(); ++f) {
			NF(f, 0) = faces[3 * f + 0];
			NF(f, 1) = faces[3 * f + 1];
			NF(f, 2) = faces[3 * f + 2];
			NKEEP(f) = keep[f];
		}

		return;
	}

	void smart_subdivide_mesh(
		const Eigen::MatrixXd& OV,
		const Eigen::MatrixXi& OF,
		Eigen::MatrixXd& NV,
		Eigen::MatrixXi& NF,
		Eigen::VectorXi& NKEEP,
		int steps)
	{

		Eigen::MatrixXd V;
		Eigen::MatrixXi F;
		Eigen::VectorXi nKEEP;

		Eigen::VectorXi KEEP = Eigen::VectorXi::Constant(OF.rows(), 0);
		subdivide_step(OV, OF, KEEP, NV, NF, NKEEP);

		if (steps > 1) {
			V.resize(NV.rows(), 3);
			V = NV;
			F.resize(NF.rows(), 3);
			F = NF;
			nKEEP.resize(NKEEP.rows());
			nKEEP = NKEEP;
		}

		for (int i = 1; i < steps; ++i) {
			subdivide_step(V, F, nKEEP, NV, NF, NKEEP);
			// update intermediate matrices
			V.resize(NV.rows(), 3);
			V = NV;
			F.resize(NF.rows(), 3);
			F = NF;
			nKEEP.resize(NKEEP.rows());
			nKEEP = NKEEP;
		}
		return;
	}

	void subdivide_mesh(
		const Eigen::MatrixXd& OV,
		const Eigen::MatrixXi& OF,
		Eigen::MatrixXd& NV,
		Eigen::MatrixXi& NF,
		int steps)
	{
		Eigen::MatrixXd V = OV;
		Eigen::MatrixXi F = OF;
		for (int i = 0; i < steps; ++i) {
			subdivide_mesh(V, F, NV, NF);
			V.setZero(NV.rows(), 3);
			F.setZero(NF.rows(), 3);
			V = NV;
			F = NF;
		}
		return;
	}
}

bool Geomlib::TriMeshSubdivide(
	const Urho3D::Variant& meshIn,
	int steps,
	Urho3D::Variant& meshOut
)
{
	if (!TriMesh_Verify(meshIn)) {
		meshOut = Urho3D::Variant();
		return false;
	}

	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	IglMeshToMatrices(meshIn, V, F);

	Eigen::MatrixXd Vd = IglFloatToDouble(V);
	Eigen::MatrixXd NVd;
	Eigen::MatrixXi NF;

	subdivide_mesh(Vd, F, NVd, NF, steps);

	Eigen::MatrixXf NV = IglDoubleToFloat(NVd);

	meshOut = TriMesh_Make(NV, NF);
	return true;
}
