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


#include "Mesh_HarmonicDeformation.h"
#include <Urho3D/Core/Variant.h>
#include <algorithm>
#include "TriMesh.h"

#pragma warning(push, 0)
#include <igl/colon.h>
#include <igl/harmonic.h>
#include <igl/readOBJ.h>
#include <igl/readDMAT.h>
#pragma warning(pop)

using namespace Urho3D;

String Mesh_HarmonicDeformation::iconTexture = "";

//geom manipulations
namespace {
};

Mesh_HarmonicDeformation::Mesh_HarmonicDeformation(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("HarmonicDeformation");
	SetFullName("Harmonic Deformation");
	SetDescription("Given some displacement vectors, the harmonic deformation field is calculated");

	AddInputSlot(
		"Mesh",
		"M",
		"Mesh to deform",
		VAR_VARIANTMAP,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Displacement vectors.",
		"V",
		"Displacement vectors. Must be parallel to index list.",
		VAR_VECTOR3,
		DataAccess::LIST,
		Vector3(0, 10, 0)
	);

	AddInputSlot(
		"Displacement indices",
		"I",
		"Index of mesh vertices to move. Must be parallel to vector list.",
		VAR_INT,
		DataAccess::LIST,
		0
	);

	AddInputSlot(
		"Harmonic exponent.",
		"K",
		"Exponent that controls the deformation field solve",
		VAR_INT,
		DataAccess::ITEM,
		2
	);

	AddOutputSlot(
		"Harmonic Displacements",
		"HD",
		"Harmonic displacemnt vectors",
		VAR_VARIANTVECTOR,
		DataAccess::LIST
	);

	AddOutputSlot(
		"Mesh",
		"DM",
		"Transformed Mesh",
		VAR_VARIANTMAP,
		DataAccess::ITEM
	);

}

void Mesh_HarmonicDeformation::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	//get data
	Variant mesh = inSolveInstance[0];
	VariantVector dispVecs = inSolveInstance[1].GetVariantVector();
	VariantVector dispIdx = inSolveInstance[2].GetVariantVector();
	int power = inSolveInstance[3].GetInt();

	//checks
	if (!TriMesh_Verify(mesh)) {
		URHO3D_LOGWARNING("M1 must be a TriMesh!");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	if (dispVecs.Empty() ||
		dispIdx.Empty())
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}


	if (dispVecs.Size() != dispIdx.Size())
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//check that we don't have the default variant lists
	if (dispVecs[0].GetType() == VAR_NONE ||
		dispIdx[0].GetType() == VAR_NONE)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//init matrices
	Eigen::MatrixXd V;
	Eigen::MatrixXi F;
	Eigen::VectorXi b;
	Eigen::MatrixXd D_bc;
	Eigen::MatrixXd D;

	//TriMeshToMatrices(mesh, V, F);
	TriMeshToDoubleMatrices(mesh, V, F);

	//collect the disp vecs and indices
	int numVecs = Min(dispVecs.Size(), dispIdx.Size());
	b.resize(numVecs);
	D_bc.resize(numVecs, 3);

	//create the handle and displacement vectors
	for (int i = 0; i < numVecs; i++)
	{
		int idx = dispIdx[i].GetInt();
		if (idx > V.rows())
		{
			URHO3D_LOGERROR("Provide an out of range index!");
			SetAllOutputsNull(outSolveInstance);
			return;
		}

		Vector3 v = dispVecs[i].GetVector3();
		D_bc.row(i) = Eigen::RowVector3d(v.x_, v.y_, v.z_);
		b[i] = idx;
	}

	//finally, proceed with calculation
	igl::harmonic(V, F, b, D_bc, power, D);

	int dRows = D.rows();
	int dCols = D.cols();

	VariantVector vecsOut;
	VariantVector verts = TriMesh_GetVertexList(mesh);
	VariantVector faces = TriMesh_GetFaceList(mesh);

	for (int i = 0; i < D.rows(); i++)
	{
		Eigen::RowVector3d v = D.row(i);
		Vector3 dV = Vector3(v.x(), v.y(), v.z());
		vecsOut.Push(dV);

		Vector3 orgVert = verts[i].GetVector3();
		verts[i] = orgVert + dV;
	}

	outSolveInstance[0] = vecsOut;
	outSolveInstance[1] = TriMesh_Make(verts, faces);
	return;
}