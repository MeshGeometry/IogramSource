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

#include "Mesh_CubicLattice.h"

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

String Mesh_CubicLattice::iconTexture = "Textures/Icons/Mesh_CubicLattice.png";

Mesh_CubicLattice::Mesh_CubicLattice(Context* context) : IoComponentBase(context, 0, 0)
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
		Vector3(0, 0, 0)
		);

	AddInputSlot(
		"Upper",
		"U",
		"Optional upper bound for box.",
		VAR_VECTOR3,
		ITEM,
		Vector3(10, 10, 10)
		);

	AddInputSlot(
		"Size",
		"dx",
		"Optional grid cell size",
		VAR_FLOAT,
		ITEM
		);

	AddInputSlot(
		"Dimensions",
		"D",
		"Vector that defines the grid dimensions",
		VAR_VECTOR3,
		ITEM,
		Vector3(3, 3, 3)
		);

	AddOutputSlot(
		"Mesh",
		"N",
		"Mesh",
		VAR_VARIANTMAP,
		ITEM
		);
}

void Mesh_CubicLattice::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	Vector3 lower = inSolveInstance[0].GetVector3();
	Vector3 upper = inSolveInstance[1].GetVector3();
	Vector3 dims = inSolveInstance[3].GetVector3();


	VariantType typeDX = inSolveInstance[2].GetType();
	float DX = 1.0f;
	if (typeDX == VariantType::VAR_FLOAT || typeDX == VariantType::VAR_INT) {
		URHO3D_LOGWARNING("Ignoring max box dimension");
		DX = inSolveInstance[2].GetFloat();
		upper = lower + DX*dims;
	}
	

	// if DX is provided

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

	//construct mesh from points
	VariantVector faces;
	int a, b, c, d, e, f, g, h;
	for (int i = 0; i < yres; i++)
	{
		for (int j = 0; j < zres; j++)
		{
			for (int k = 0; k <xres; k++)
			{
				a = k + j * (zres + 1) + i *(zres + 1) * (xres + 1);
				b = a + 1;
				c = b + xres + 1;
				d = b + xres;
				e = a + (xres + 1 )*(zres + 1);
				f = b + (xres + 1 )*(zres + 1);
				g = c + (xres + 1 )*(zres + 1);
				h = d + (xres + 1 )*(zres + 1);

				//bottom
				faces.Push(a); faces.Push(c); faces.Push(b);
				faces.Push(a); faces.Push(d); faces.Push(c);

				//top
				faces.Push(e); faces.Push(f); faces.Push(g);
				faces.Push(e); faces.Push(g); faces.Push(h);

				//side
				faces.Push(a); faces.Push(f); faces.Push(e);
				faces.Push(a); faces.Push(b); faces.Push(f);

				//side
				faces.Push(b); faces.Push(g); faces.Push(f);
				faces.Push(b); faces.Push(c); faces.Push(g);

				//side
				faces.Push(c); faces.Push(d); faces.Push(h);
				faces.Push(c); faces.Push(h); faces.Push(g);

				//side
				faces.Push(d); faces.Push(a); faces.Push(e);
				faces.Push(d); faces.Push(e); faces.Push(h);
			}
		}
	}

	// now morph and add the tetrahedra. 
	// for each cell, map the tetrahedra into that cell.
	VariantVector tets;
    InitializeTetGeometry();
	int counter = pts.Size();
	Vector3 shrink = Vector3(dx / 4, dy / 4, dz / 4);
	for (int i = 0; i < yres; i++)
	{
		for (int j = 0; j < zres; j++)
		{
			for (int k = 0; k < xres; k++)
			{
                Vector3 origin = lower + Vector3(k*dx, i*dy, j*dz);
				PushTetVerts(pts, origin, shrink);
				PushTetIds(tets, counter);
				counter = pts.Size();
			}
		}
	}

	Variant tri_mesh = TriMesh_Make(pts, faces);
	// update the tets field.
	VariantMap mesh_map = tri_mesh.GetVariantMap();
	mesh_map["tetrahedra"] = tets;

	outSolveInstance[0] = Variant(mesh_map);
}

void Mesh_CubicLattice::InitializeTetGeometry()
{
	// vertices

	// Z = 0
	tet_verts.Push(Vector3(0, 0, 0));
	tet_verts.Push(Vector3(2, 0, 0));
	tet_verts.Push(Vector3(4, 0, 0));
	tet_verts.Push(Vector3(0, 2, 0));
	tet_verts.Push(Vector3(2, 2, 0));
	tet_verts.Push(Vector3(4, 2, 0));
	tet_verts.Push(Vector3(0, 4, 0));
	tet_verts.Push(Vector3(2, 4, 0));
	tet_verts.Push(Vector3(4, 4, 0));

	// Z = 2
	tet_verts.Push(Vector3(0, 0, 2));
	tet_verts.Push(Vector3(2, 0, 2));
	tet_verts.Push(Vector3(4, 0, 2));
	tet_verts.Push(Vector3(0, 2, 2));
	tet_verts.Push(Vector3(2, 2, 2));
	tet_verts.Push(Vector3(4, 2, 2));
	tet_verts.Push(Vector3(0, 4, 2));
	tet_verts.Push(Vector3(2, 4, 2));
	tet_verts.Push(Vector3(4, 4, 2));

	// Z = 4
	tet_verts.Push(Vector3(0, 0, 4));
	tet_verts.Push(Vector3(2, 0, 4));
	tet_verts.Push(Vector3(4, 0, 4));
	tet_verts.Push(Vector3(0, 2, 4));
	tet_verts.Push(Vector3(2, 2, 4));
	tet_verts.Push(Vector3(4, 2, 4));
	tet_verts.Push(Vector3(0, 4, 4));
	tet_verts.Push(Vector3(2, 4, 4));
	tet_verts.Push(Vector3(4, 4, 4));

	// ids
	tet_ids.Push(Vector4(1, 13, 4, 0));
	tet_ids.Push(Vector4(3, 13, 4, 0));
	tet_ids.Push(Vector4(3, 13, 4, 6));
	tet_ids.Push(Vector4(7, 13, 4, 6));
	tet_ids.Push(Vector4(7, 13, 4, 8));
	tet_ids.Push(Vector4(5, 13, 4, 8));
	tet_ids.Push(Vector4(5, 13, 4, 2));
	tet_ids.Push(Vector4(1, 13, 4, 2));

	tet_ids.Push(Vector4(11, 13, 14, 2));
	tet_ids.Push(Vector4(5, 13, 14, 2));
	tet_ids.Push(Vector4(5, 13, 14, 8));
	tet_ids.Push(Vector4(17, 13, 14, 8));
	tet_ids.Push(Vector4(17, 13, 14, 26));
	tet_ids.Push(Vector4(23, 13, 14, 26));
	tet_ids.Push(Vector4(23, 13, 14, 20));
	tet_ids.Push(Vector4(11, 13, 14, 20));

	tet_ids.Push(Vector4(7, 13, 16, 6));
	tet_ids.Push(Vector4(15, 13, 16, 6));
	tet_ids.Push(Vector4(15, 13, 16, 24));
	tet_ids.Push(Vector4(25, 13, 16, 24));
	tet_ids.Push(Vector4(25, 13, 16, 26));
	tet_ids.Push(Vector4(17, 13, 16, 26));
	tet_ids.Push(Vector4(17, 13, 16, 8));
	tet_ids.Push(Vector4(7, 13, 16, 8));

	tet_ids.Push(Vector4(19, 13, 22, 20));
	tet_ids.Push(Vector4(23, 13, 22, 20));
	tet_ids.Push(Vector4(23, 13, 22, 26));
	tet_ids.Push(Vector4(25, 13, 22, 26));
	tet_ids.Push(Vector4(25, 13, 22, 24));
	tet_ids.Push(Vector4(21, 13, 22, 24));
	tet_ids.Push(Vector4(21, 13, 22, 18));
	tet_ids.Push(Vector4(19, 13, 22, 18));

	tet_ids.Push(Vector4(9, 13, 12, 18));
	tet_ids.Push(Vector4(21, 13, 12, 18));
	tet_ids.Push(Vector4(21, 13, 12, 24));
	tet_ids.Push(Vector4(15, 13, 12, 24));
	tet_ids.Push(Vector4(15, 13, 12, 6));
	tet_ids.Push(Vector4(3, 13, 12, 6));
	tet_ids.Push(Vector4(3, 13, 12, 0));
	tet_ids.Push(Vector4(9, 13, 12, 0));

	tet_ids.Push(Vector4(19, 13, 10, 18));
	tet_ids.Push(Vector4(9, 13, 10, 18));
	tet_ids.Push(Vector4(9, 13, 10, 0));
	tet_ids.Push(Vector4(1, 13, 10, 0));
	tet_ids.Push(Vector4(1, 13, 10, 2));
	tet_ids.Push(Vector4(11, 13, 10, 2));
	tet_ids.Push(Vector4(11, 13, 10, 20));
	tet_ids.Push(Vector4(19, 13, 10, 20));
}

int Mesh_CubicLattice::PushTetVerts(VariantVector& verts, Vector3 origin, Urho3D::Vector3 shrink)
{
    
	for (int i = 0; i < tet_verts.Size(); ++i) {
		Vector3 tv = tet_verts[i].GetVector3();
		Vector3 v = origin + Vector3(shrink.x_*tv.x_, shrink.y_*tv.y_, shrink.z_*tv.z_);
		verts.Push(v);
	}
	return verts.Size();
}

int Mesh_CubicLattice::PushTetIds(VariantVector& tets, int vert_counter)
{
	for (int i = 0; i < tet_ids.Size(); ++i) {
		Vector4 T = tet_ids[i].GetVector4();
		tets.Push(vert_counter + (int)T.x_);
		tets.Push(vert_counter + (int)T.y_);
		tets.Push(vert_counter + (int)T.z_);
		tets.Push(vert_counter + (int)T.w_);
	}

	return tets.Size();
}
