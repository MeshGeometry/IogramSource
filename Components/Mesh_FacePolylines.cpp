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


#include "Mesh_FacePolylines.h"

#include <assert.h>


#include "ConversionUtilities.h"
#include "TriMesh.h"
#include "Polyline.h"



using namespace Urho3D;

String Mesh_FacePolylines::iconTexture = "Textures/Icons/Mesh_FacePolylines.png";

Mesh_FacePolylines::Mesh_FacePolylines(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("FacePolylines");
	SetFullName("Face Polylines");
	SetDescription("Returns the face polylines from a mesh");

	AddInputSlot(
		"Mesh",
		"M",
		"Mesh",
		VAR_VARIANTMAP,
		ITEM
	);

	AddOutputSlot(
		"Polylines",
		"P",
		"Polylines",
		VAR_VARIANTMAP,
		LIST
	);

}

void Mesh_FacePolylines::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{


	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	Variant inMesh = inSolveInstance[0];
	if (!TriMesh_Verify(inMesh)) {
		URHO3D_LOGWARNING("M must be a valid mesh.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//assuming tri mesh here...
	VariantVector faces = TriMesh_GetFaceList(inMesh);
	VariantVector verts = TriMesh_GetVertexList(inMesh);
	VariantVector polys;
	for (int i = 0; i < faces.Size() / 3; i++)
	{
		int a = faces[3 * i].GetInt();
		int b = faces[3 * i + 1].GetInt();
		int c = faces[3 * i + 2].GetInt();

		Vector3 vA = verts[a].GetVector3();
		Vector3 vB = verts[b].GetVector3();
		Vector3 vC = verts[c].GetVector3();

		VariantVector pVerts;
		pVerts.Push(vA);
		pVerts.Push(vB);
		pVerts.Push(vC);
		//close
		pVerts.Push(vA);

		polys.Push(Polyline_Make(pVerts));

	}

	// ASSIGN OUTPUTS

	outSolveInstance[0] = polys;
}
