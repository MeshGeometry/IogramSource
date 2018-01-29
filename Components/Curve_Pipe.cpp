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


#include "Curve_Pipe.h"
#include "StockGeometries.h"
#include "Geomlib_TransformVertexList.h"
#include "Polyline.h"
#include "TriMesh.h"
#include "Geomlib_TriangulatePolygon.h"
#include "Geomlib_TriMeshThicken.h"
#include "Geomlib_JoinMeshes.h"
#include "Geomlib_PolylineLoft.h"

using namespace Urho3D;

String Curve_Pipe::iconTexture = "Textures/Icons/Curve_Pipe.png";



Curve_Pipe::Curve_Pipe(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("Pipe");
	SetFullName("Pipe a polyline");
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
		20
	);

	AddInputSlot(
		"Radius",
		"R",
		"Pipe radius",
		VAR_FLOAT,
		ITEM,
		1.0f
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

void Curve_Pipe::SolveInstance(
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
	float r = inSolveInstance[2].GetFloat();

	r = Max(r, 0.001f);
	//scale we get from the inputs
	tr.SetScale(Vector3(r, 1.0f, r));
    //Quaternion polyQ;
    //polyQ.FromRotationTo(Vector3(0,0,1), Vector3(0,1,0));
    //tr.SetRotation(polyQ.RotationMatrix());

	//create the base polygon
	Variant base_polygon = MakeRegularPolygon(n);
	VariantVector baseVerts = Polyline_GetVertexList(base_polygon);
	VariantVector newVerts = Geomlib::TransformVertexList(tr, baseVerts);
	Quaternion polyQ;
	polyQ.FromRotationTo(Vector3(0, 0, 1), Vector3(0, 1, 0));
	tr.SetRotation(polyQ.RotationMatrix());
	newVerts = Geomlib::TransformVertexList(tr, newVerts);

	//this is the basic section poly
	Variant basePoly = Polyline_Make(newVerts);
	Polyline_Close(basePoly);

	//translation is constructed from the start point of the reail
	VariantVector railVerts = Polyline_ComputeSequentialVertexList(curve);

	//now loop over all the verts and construct transform
	// add transformed basePoly to list of sections, then loft them
	VariantVector sections;
	int N = railVerts.Size();
	for (int i = 0; i < N; ++i)
	{
		
		Vector3 start = railVerts[i].GetVector3();

		tr.SetTranslation(start);

		Quaternion rot;
		Vector3 dir;
		if (i == 0) {// endpoint
			dir = railVerts[i + 1].GetVector3() - start;
            //rot.FromRotationTo(dir, Vector3::UP);
			rot.FromLookRotation(dir, Vector3::UP);
            
		}
		else if (i > 0 && i < N - 1) { // interior
			Vector3 A = start - railVerts[i - 1].GetVector3();
			Vector3 B = start - railVerts[i + 1].GetVector3();
			Vector3 C = A.CrossProduct(B);
			Vector3 bisector = (A + B).Normalized();
			dir = bisector.CrossProduct(C);

//            rot.FromRotationTo(dir, Vector3::UP);
			rot.FromLookRotation(dir, Vector3::UP);
            
		}
		else if (i == N - 1) {
			dir = (start - railVerts[N - 2].GetVector3());
           // rot.FromRotationTo(dir, Vector3::UP);
			rot.FromLookRotation(dir, Vector3::UP);
		}

		tr.SetRotation(rot.RotationMatrix());
        

		VariantVector transVerts = Geomlib::TransformVertexList(tr, newVerts);
		Variant transPoly = Polyline_Make(transVerts);
		Polyline_Close(transPoly);
		sections.Push(transPoly);

	}

	Variant meshOut;
	Geomlib::PolylineLoft(sections, meshOut);

	outSolveInstance[0] = meshOut;
	//outSolveInstance[1] = baseSectionMesh;
}
