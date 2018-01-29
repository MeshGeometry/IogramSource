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


#include "Geometry_ProjectOnto.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Plane.h>

#include "TriMesh.h"
#include "Polyline.h"
#include "igl/ray_mesh_intersect.h"

using namespace Urho3D;
using namespace Eigen;
String Geometry_ProjectOnto::iconTexture = "Textures/Icons/Geometry_ProjectOnto.png";


Geometry_ProjectOnto::Geometry_ProjectOnto(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("ProjectOnto");
	SetFullName("ProjectOnto");
	SetDescription("Projects geometry on to other geometry");

	AddInputSlot(
		"Source",
		"S",
		"Source geometry",
		VAR_VARIANTMAP,
		ITEM
		);

	AddInputSlot(
		"Direction",
		"D",
		"Direction",
		VAR_VECTOR3,
		ITEM
		);

	AddInputSlot(
		"Fallback",
		"F",
		"Fallback parametr to use if no intersection",
		VAR_FLOAT,
		ITEM,
		0.0f
		);

	AddInputSlot(
		"Target",
		"T",
		"Target geometry",
		VAR_VARIANTMAP,
		ITEM
		);

	AddOutputSlot(
		"Geo",
		"G",
		"Result geometry",
		VAR_VARIANTMAP,
		ITEM
		);

	AddOutputSlot(
		"Points",
		"P",
		"Hit points",
		VAR_VECTOR3,
		LIST
		);
}

void Geometry_ProjectOnto::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	Variant sourceGeom = inSolveInstance[0];
	Vector3 direction = inSolveInstance[1].GetVector3();
	Variant targetGeom = inSolveInstance[3];
	float defaultParam = inSolveInstance[2].GetFloat();
	MatrixXf V;
	MatrixXi F;
	TriMeshToMatrices(targetGeom, V, F);

	if (TriMesh_Verify(sourceGeom) && TriMesh_Verify(targetGeom))
	{
		VariantVector verts = TriMesh_GetVertexList(sourceGeom);
		VariantVector hitPoints;
		igl::Hit hit;
		Vector3d dir(direction.x_, direction.y_, direction.z_);
		int numVerts = verts.Size();
		for (int i = 0; i < numVerts; i++)
		{
			//cast ray
			Vector3 currVert = verts[i].GetVector3();
			Vector3d origin(currVert.x_, currVert.y_, currVert.z_);
			bool res = igl::ray_mesh_intersect(origin, dir, V, F, hit);
			float rayParam = defaultParam;
			if (res)
			{
				rayParam = hit.t;
				hitPoints.Push(currVert + rayParam * direction);
			}

			Vector3 newPoint = currVert + rayParam * direction;
			verts[i] = newPoint;
		}

		//create the new mesh
		Variant projMesh = TriMesh_Make(verts, TriMesh_GetFaceList(sourceGeom));
		outSolveInstance[0] = projMesh;
		outSolveInstance[1] = hitPoints;
		return;
	}
	else if (Polyline_Verify(sourceGeom) && TriMesh_Verify(targetGeom))
	{
		VariantVector verts = Polyline_GetVertexList(sourceGeom);
		igl::Hit hit;
		Vector3d dir(direction.x_, direction.y_, direction.z_);
		int numVerts = verts.Size();
		VariantVector hitPoints;
		for (int i = 0; i < numVerts; i++)
		{
			//cast ray
			Vector3 currVert = verts[i].GetVector3();
			Vector3d origin(currVert.x_, currVert.y_, currVert.z_);
			bool res = igl::ray_mesh_intersect(origin, dir, V, F, hit);
			float rayParam = defaultParam;
			if (res)
			{
				rayParam = hit.t;
				hitPoints.Push(currVert + rayParam * direction);
			}

			Vector3 newPoint = currVert + rayParam * direction;
			verts[i] = newPoint;

		}

		//create the new mesh
		Variant projPoly = Polyline_Make(verts);
		outSolveInstance[0] = projPoly;
		outSolveInstance[1] = hitPoints;
		return;
	}
	else if (sourceGeom.GetType() == VAR_VECTOR3 && TriMesh_Verify(targetGeom))
	{
		//cast ray
		Vector3 currVert = sourceGeom.GetVector3();
		Vector3d origin(currVert.x_, currVert.y_, currVert.z_);
		igl::Hit hit;
		Vector3d dir(direction.x_, direction.y_, direction.z_);
		bool res = igl::ray_mesh_intersect(origin, dir, V, F, hit);
		VariantVector hitPoints;
		float rayParam = defaultParam;
		if (res)
		{
			rayParam = hit.t;
			hitPoints.Push(currVert + rayParam * direction);
		}

		Vector3 newPoint = currVert + rayParam * direction;

		outSolveInstance[0] = newPoint;
		outSolveInstance[1] = hitPoints;

		return;
	}
	else
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}
}
