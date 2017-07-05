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


#include "Mesh_Pipe.h"
#include "StockGeometries.h"
#include "Geomlib_TransformVertexList.h"
#include "Polyline.h"
#include "TriMesh.h"
#include "Geomlib_TriangulatePolygon.h"
#include "Geomlib_TriMeshThicken.h"
#include "Geomlib_JoinMeshes.h"

using namespace Urho3D;

String Mesh_Pipe::iconTexture = "Textures/Icons/Mesh_Pipe.png";

namespace
{
	Variant CreateBaseSectionMesh(Variant sectionCurve)
	{
		Variant mesh;
		VariantVector holes;
		bool res = Geomlib::TriangulatePolygon(sectionCurve, holes, mesh);
		return mesh;
	}

	Variant ExtrudeSectionMeshAlongVector(Variant sectionMesh, Vector3 start, Vector3 end, float extension)
	{
		
		Matrix3x4 xform;
		Vector3 direction = end - start;

		//apply extensions
		start = start - extension * direction.Normalized();
		end = end + extension * direction.Normalized();
		direction = end - start;


		//figure out rotation from world up to direction
		Quaternion lookRot;
		lookRot.FromLookRotation(direction, Vector3::UP);
		Quaternion rot;
		rot.FromEulerAngles(90.f, 0.f, 0.f);
		Matrix3 mat = (lookRot * rot).RotationMatrix();
		xform.SetRotation(mat);

		xform.SetTranslation(start);

		//transform verts
		VariantMap poly = sectionMesh.GetVariantMap();
		VariantVector oldVerts = poly["vertices"].GetVariantVector();
		VariantVector newVerts = Geomlib::TransformVertexList(xform, oldVerts);

		//apply
		poly["vertices"] = newVerts;
	
		//thicken	
		Variant thickMesh;
		Geomlib::TriMeshThicken(poly, direction.Length(), thickMesh);

		return thickMesh;
	}
}

Mesh_Pipe::Mesh_Pipe(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("Polygon");
	SetFullName("Construct Polygon");
	SetDescription("Construct a polygon with n sides");

	AddInputSlot(
		"Curve",
		"C",
		"Curve to pipe.",
		VAR_VARIANTMAP,
		ITEM
	);

	AddInputSlot(
		"Sides",
		"S",
		"Number of sides in section.",
		VAR_INT,
		ITEM,
		4
	);

	AddInputSlot(
		"DeltaX",
		"X",
		"Stretch in local X direction",
		VAR_FLOAT,
		ITEM,
		1.0f
	);

	AddInputSlot(
		"DeltaY",
		"Y",
		"Stretch in local Y direction",
		VAR_FLOAT,
		ITEM,
		1.0f
	);

	AddInputSlot(
		"Extension",
		"E",
		"Extends the segment by this amount.",
		VAR_FLOAT,
		ITEM,
		0.0f
	);

	AddOutputSlot(
		"Mesh",
		"M",
		"Meshed curved with given section.",
		VAR_VARIANTMAP,
		ITEM
	);

	AddOutputSlot(
		"Debug",
		"D",
		"Meshed curved with given section.",
		VAR_VARIANTMAP,
		ITEM
	);


}

void Mesh_Pipe::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	//get the data
	Variant curve = inSolveInstance[0];
	int n = inSolveInstance[1].GetInt();

	if (n <= 2 || !Polyline_Verify(curve)) {
		URHO3D_LOGWARNING("N must be >= 3 and C must be a valid polyline");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//create the section transformation
	Urho3D::Matrix3x4 tr = Matrix3x4::IDENTITY;
	float dx = inSolveInstance[2].GetFloat();
	float dy = inSolveInstance[3].GetFloat();

	dx = Max(dx, 0.001f);
	dy = Max(dy, 0.001f);

	float extension = inSolveInstance[4].GetFloat();

	//rotation is given since polygons are create in XZ plane
	//tr.SetRotation(Quaternion(-90.0f, Vector3::RIGHT).RotationMatrix());

	//scale we get from the inputs
	tr.SetScale(Vector3(dx, 1.0f, dy));

	//create the base polygon
	Variant base_polygon = MakeRegularPolygon(n);
	VariantVector baseVerts = Polyline_GetVertexList(base_polygon);
	VariantVector newVerts = Geomlib::TransformVertexList(tr, baseVerts);

	//this is the basic section poly
	Variant basePoly = Polyline_Make(newVerts);
	Polyline_Close(basePoly);
	Variant baseSectionMesh = CreateBaseSectionMesh(basePoly);

	//translation is constructed from the start point of the reail
	VariantVector railVerts = Polyline_ComputeSequentialVertexList(curve);

	//now loop over all the verts and extrude
	VariantVector meshes;
	for (int i = 0; i < railVerts.Size() - 1; i++)
	{
		Vector3 start = railVerts[i].GetVector3();
		Vector3 end = railVerts[i + 1].GetVector3();

		Variant extrusion = ExtrudeSectionMeshAlongVector(baseSectionMesh, start, end, extension);
		meshes.Push(extrusion);

	}

	Variant meshOut = Geomlib::JoinMeshes(meshes);

	outSolveInstance[0] = meshOut;
	outSolveInstance[1] = baseSectionMesh;
}
