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

#include "Vector_Grid3D.h"

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
#include "TriMesh.h"

using namespace Urho3D;

String Vector_Grid3D::iconTexture = "Textures/Icons/Vector_Grid3D.png";

Vector_Grid3D::Vector_Grid3D(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("Grid3D");
	SetFullName("Grid3D");
	SetDescription("Create a mesh from a grid of values");

	AddInputSlot(
		"Lower",
		"L",
		"Lower bound for box.",
		VAR_VECTOR3,
		ITEM,
		Vector3(0,0,0)
	);

	AddInputSlot(
		"Upper",
		"U",
		"Upper bound for box.",
		VAR_VECTOR3,
		ITEM,
		Vector3(10, 10, 10)
	);

	AddInputSlot(
		"Dimensions",
		"D",
		"Vector that defines the grid dimensions",
		VAR_VECTOR3,
		ITEM,
		Vector3(10, 10, 10)
	);

	AddOutputSlot(
		"Points",
		"P",
		"Points",
		VAR_VARIANTVECTOR,
		LIST
	);
}

void Vector_Grid3D::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Vector3 lower = inSolveInstance[0].GetVector3();
	Vector3 upper = inSolveInstance[1].GetVector3();
	Vector3 dims = inSolveInstance[2].GetVector3();

	int xres = (int)dims.x_;
	int yres = (int)dims.y_;
	int zres = (int)dims.z_;

	float dx = (upper.x_ - lower.x_) / (float)xres;
	float dy = (upper.y_ - lower.y_) / (float)yres;
	float dz = (upper.z_ - lower.z_) / (float)zres;

	//check
	if (xres*yres*zres <= 0)
	{
		URHO3D_LOGINFO("Invalid grid dimensions!");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	VariantVector pts;
	//pts.Resize(xres * yres * zres);

	for (int i = 0; i <= yres; i++)
	{
		for (int j = 0; j <= zres; j++)
		{
			for (int k = 0; k <= xres; k++)
			{
				Vector3 v = lower + Vector3(k*dx, i*dy, j*dz);
				pts.Push(v);
			}
		}
	}

	outSolveInstance[0] = pts;
}