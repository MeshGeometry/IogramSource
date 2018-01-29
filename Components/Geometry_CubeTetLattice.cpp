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


#include "Geometry_CubeTetLattice.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Plane.h>

#include "TriMesh.h"
#include "Polyline.h"
#include "Geomlib_ConstructTransform.h"
#include "igl/ray_mesh_intersect.h"
#include "Geomlib_JoinMeshes.h"

using namespace Urho3D;

String Geometry_CubeTetLattice::iconTexture = "Textures/Icons/Geometry_CubeTetLattice.png";

namespace
{
	Variant CreateBaseUnit(float scale)
	{
		Variant base;

		//define base unit
		VariantVector bv;
		bv.Push(Vector3(0.0, 0.0, 0.0));
		bv.Push(Vector3(0.5, 0.0, 0.0));
		bv.Push(Vector3(0.5, 0.5, 0.5));
		bv.Push(Vector3(0.5, 0.0, 0.5));
		bv.Push(Vector3(0.5, -0.5, 0.5));

		VariantVector bi;
		bi.Push(0); bi.Push(1); bi.Push(2);
		bi.Push(0); bi.Push(3); bi.Push(2);
		bi.Push(0); bi.Push(2); bi.Push(3);
		bi.Push(1); bi.Push(3); bi.Push(2);

		bi.Push(0); bi.Push(1); bi.Push(3);
		bi.Push(0); bi.Push(3); bi.Push(4);
		bi.Push(0); bi.Push(4); bi.Push(1);
		bi.Push(1); bi.Push(4); bi.Push(3);

		base = TriMesh_Make(bv, bi);

		//now rotate to fill out cube face
		Matrix3x4 xform = Matrix3x4::IDENTITY;
		Urho3D::Quaternion rot;
		rot.FromAngleAxis(90.0f, Vector3::RIGHT);
		xform.SetRotation(rot.RotationMatrix());

		Variant r90 = TriMesh_ApplyTransform(base, xform);
		Variant r180 = TriMesh_ApplyTransform(r90, xform);
		Variant r270 = TriMesh_ApplyTransform(r180, xform);

		VariantVector parts;
		parts.Push(base);
		parts.Push(r90);
		parts.Push(r180);
		parts.Push(r270);

		//join
		Variant joined = Geomlib::JoinMeshes(parts);
		xform = Matrix3x4::IDENTITY;
		xform.SetScale(scale);
		return TriMesh_ApplyTransform(joined, xform);
	}

	Variant AppendFull(const Variant&base, Vector3 center)
	{

		Matrix3x4 xform = Matrix3x4::IDENTITY;
		Quaternion rot = Quaternion::IDENTITY;
		VariantVector parts;
		rot.FromAngleAxis(90, Vector3::UP);
		xform.SetRotation(rot.RotationMatrix());
		parts.Push(TriMesh_ApplyTransform(base, xform));

		rot.FromAngleAxis(-90, Vector3::UP);
		xform.SetRotation(rot.RotationMatrix());
		parts.Push(TriMesh_ApplyTransform(base, xform));


		rot.FromAngleAxis(-90, Vector3::FORWARD);
		xform.SetRotation(rot.RotationMatrix());
		parts.Push(TriMesh_ApplyTransform(base, xform));

		rot.FromAngleAxis(90, Vector3::FORWARD);
		xform.SetRotation(rot.RotationMatrix());
		parts.Push(TriMesh_ApplyTransform(base, xform));

		rot.FromAngleAxis(180, Vector3::UP);
		xform.SetRotation(rot.RotationMatrix());
		parts.Push(TriMesh_ApplyTransform(base, xform));

		parts.Push(base);

		//join the parts
		Variant joined = Geomlib::JoinMeshes(parts);

		//translate
		xform = Matrix3x4::IDENTITY;
		xform.SetTranslation(center);

		return TriMesh_ApplyTransform(joined, xform);
	}

	Variant AppendByFlag(const Variant& base, Vector3 center, int flag)
	{
		Matrix3x4 xform = Matrix3x4::IDENTITY;
		Quaternion rot = Quaternion::IDENTITY;
		VariantVector parts;

		//check if full unit is needed
		if ((flag & 1) && (flag & 2) ||
			(flag & 4) && (flag & 8) ||
			(flag & 16) && (flag & 32))
		{
			return AppendFull(base, center);
		}


		if (flag & 1) //-z face
		{
			rot.FromAngleAxis(90, Vector3::UP);
			xform.SetRotation(rot.RotationMatrix());
			parts.Push(TriMesh_ApplyTransform(base, xform));
		}
		if (flag & 2) //+x face
		{
			rot.FromAngleAxis(-90, Vector3::UP);
			xform.SetRotation(rot.RotationMatrix());
			parts.Push(TriMesh_ApplyTransform(base, xform));

		}
		if (flag & 4) //-y face
		{
			rot.FromAngleAxis(-90, Vector3::FORWARD);
			xform.SetRotation(rot.RotationMatrix());
			parts.Push(TriMesh_ApplyTransform(base, xform));
		}
		if (flag & 8) //+y face
		{

			rot.FromAngleAxis(90, Vector3::FORWARD);
			xform.SetRotation(rot.RotationMatrix());
			parts.Push(TriMesh_ApplyTransform(base, xform));
		}
		if (flag & 16) //-x face
		{
			rot.FromAngleAxis(180, Vector3::UP);
			xform.SetRotation(rot.RotationMatrix());
			parts.Push(TriMesh_ApplyTransform(base, xform));
		}
		if (flag & 32) //+x face
		{
			//nothing to do
			parts.Push(base);
		}

		//join the parts
		Variant joined = Geomlib::JoinMeshes(parts);

		//translate
		xform = Matrix3x4::IDENTITY;
		xform.SetTranslation(center);

		return TriMesh_ApplyTransform(joined, xform);
	}
}


Geometry_CubeTetLattice::Geometry_CubeTetLattice(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("CubeTetLattice");
	SetFullName("CubeTetLattice");
	SetDescription("Creates a 48 point cube-tet lattice.");

	AddInputSlot(
		"Centers",
		"C",
		"Center points.",
		VAR_VECTOR3,
		LIST
	);

	AddInputSlot(
		"Flags",
		"F",
		"Flags",
		VAR_INT,
		LIST
	);

	AddInputSlot(
		"Scale",
		"S",
		"Scale",
		VAR_FLOAT,
		ITEM,
		1.0f
	);

	AddOutputSlot(
		"Geo",
		"G",
		"Result geometry",
		VAR_VARIANTMAP,
		ITEM
	);


}

void Geometry_CubeTetLattice::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{

	VariantVector centers = inSolveInstance[0].GetVariantVector();
	VariantVector flags = inSolveInstance[1].GetVariantVector();
	float scale = inSolveInstance[2].GetFloat();

	//create base
	Variant base = CreateBaseUnit(scale);

	//resulting mesh
	VariantVector latticeParts;

	//loop through centers
	int numCenters = centers.Size();
	int numFlags = flags.Size();
	for (int i = 0; i < numCenters; i++)
	{
		Vector3 c = centers[i].GetVector3();
		int f = 0;
		if (i < numFlags)
			f = flags[i].GetInt();

		//based on the flag, add the appropriate verts/tets
		Variant newPart = AppendByFlag(base, c, f);

		//push
		latticeParts.Push(newPart);
	}

	//append to mesh
	Variant lattice = Geomlib::JoinMeshes(latticeParts);

	outSolveInstance[0] = lattice;

}
