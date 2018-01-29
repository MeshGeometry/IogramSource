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

#include "Mesh_MarchingCubes.h"

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

#include "igl/copyleft/marching_cubes.h"
#include "igl/signed_distance.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_MarchingCubes::iconTexture = "Textures/Icons/Mesh_MarchingCubes.png";

Mesh_MarchingCubes::Mesh_MarchingCubes(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("MarchingCubes");
	SetFullName("MarchingCubes");
	SetDescription("Create a mesh from a grid of values");

	AddInputSlot(
		"Points",
		"P",
		"Grid points",
		VAR_VECTOR3,
		LIST
	);

	AddInputSlot(
		"Values",
		"V",
		"Values at grid points",
		VAR_FLOAT,
		LIST
	);

	AddInputSlot(
		"Dimensions",
		"D",
		"Vector that defines the grid dimensions",
		VAR_VECTOR3,
		ITEM,
		Vector3(10,10,10)
	);

	AddInputSlot(
		"Level",
		"L",
		"Level set to evaluate",
		VAR_FLOAT,
		ITEM,
		0.0f
	);

	AddOutputSlot(
		"Mesh",
		"M",
		"Mesh",
		VAR_VARIANTMAP,
		ITEM
	);
}

void Mesh_MarchingCubes::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	VariantVector pts = inSolveInstance[0].GetVariantVector();
	VariantVector vals = inSolveInstance[1].GetVariantVector();
	Vector3 dims = inSolveInstance[2].GetVector3();
	float level = inSolveInstance[3].GetFloat();

	//do a check
	if (dims.x_ * dims.y_ * dims.z_ != pts.Size() ||
		dims.x_ * dims.y_ * dims.z_ != vals.Size())
	{
		URHO3D_LOGINFO("Grid dimensions don't match point of value list!");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	int xres = (int)dims.x_;
	int yres = (int)dims.y_;
	int zres = (int)dims.z_;
	
	Eigen::MatrixXf iPoints(xres * yres * zres, 3);
	Eigen::VectorXf iValues(xres * yres * zres);

	//fill grid points
	for (int i = 0; i < pts.Size(); i++)
	{
		Vector3 v = pts[i].GetVector3();
		iPoints.row(i) = Eigen::RowVector3f(v.x_, v.y_, v.z_);

		float val = vals[i].GetFloat();
		iValues(i) = val;
	}

	//fill grid values
	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	igl::copyleft::marching_cubes(iValues, iPoints, xres, yres, zres, V, F);

	outSolveInstance[0] = TriMesh_Make(V, F);

	
}