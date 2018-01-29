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

#include "Mesh_SignedDistance.h"

#include <assert.h>

#include <iostream>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "igl/signed_distance.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_SignedDistance::iconTexture = "Textures/Icons/Mesh_SignedDistance.png";

Mesh_SignedDistance::Mesh_SignedDistance(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("MarchingCubes");
	SetFullName("MarchingCubes");
	SetDescription("Create a mesh from a grid of values");

	AddInputSlot(
		"Points",
		"P",
		"Query points",
		VAR_VECTOR3,
		LIST
	);

	AddInputSlot(
		"Mesh",
		"M",
		"Mesh to find distance to",
		VAR_VARIANTMAP,
		ITEM
	);

	AddOutputSlot(
		"Distances",
		"D",
		"Distances",
		VAR_VARIANTVECTOR,
		LIST
	);

	AddOutputSlot(
		"ClosetsPts",
		"CP",
		"ClosestPoints",
		VAR_VARIANTVECTOR,
		LIST
	);

}

void Mesh_SignedDistance::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	VariantVector pts = inSolveInstance[0].GetVariantVector();
	Variant mesh = inSolveInstance[1];

	//do a check
	if (!TriMesh_Verify(mesh))
	{
		URHO3D_LOGINFO("Signed distance requires a mesh");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Eigen::MatrixXd iPts(pts.Size(), 3);
	for (int i = 0; i < pts.Size(); i++)
	{
		Vector3 v = pts[i].GetVector3();
		Eigen::RowVector3d rv(v.x_, v.y_, v.z_);
		iPts.row(i) = rv;
	}

	Eigen::MatrixXd V;
	Eigen::MatrixXi F;

	TriMeshToDoubleMatrices(mesh, V, F);

	Eigen::VectorXd S;
	Eigen::VectorXi I;
	Eigen::MatrixXd C;
	Eigen::MatrixXd N;

	igl::signed_distance(iPts, V, F, igl::SignedDistanceType::SIGNED_DISTANCE_TYPE_DEFAULT, S, I, C, N);

	VariantVector dOut;
	VariantVector ptsOut;
	for (int i = 0; i < S.rows(); i++)
	{
		dOut.Push( (float)S(i));
		Vector3 cp(C.row(i).x(), C.row(i).y(), C.row(i).z());
		ptsOut.Push(cp);
	}

	outSolveInstance[0] = dOut;
	outSolveInstance[1] = ptsOut;

}