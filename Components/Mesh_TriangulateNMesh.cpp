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


#include "Mesh_TriangulateNMesh.h"
#include <Urho3D/Core/Variant.h>
#include "ConversionUtilities.h"
#include "NMesh.h"

using namespace Urho3D;

String Mesh_TriangulateNMesh::iconTexture = "Textures/Icons/Mesh_TriangulateNMesh.png";

Mesh_TriangulateNMesh::Mesh_TriangulateNMesh(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("TriangulateNMesh");
	SetFullName("Triangulate NMesh");
	SetDescription("Triangulates a quad or N-mesh");

	AddInputSlot(
		"Mesh",
		"M",
		"Mesh to triangulate",
		VAR_VARIANTMAP,
		ITEM
	);

	AddOutputSlot(
		"MeshOut",
		"TM",
		"TriMesh out",
		VAR_VARIANTMAP,
		ITEM
	);

}

void Mesh_TriangulateNMesh::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{


	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	Variant inMesh = inSolveInstance[0];

	if (inMesh.GetType() == VAR_NONE)
	{
		URHO3D_LOGWARNING("M must be a valid mesh.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	if (!NMesh_Verify(inMesh)) {
		URHO3D_LOGWARNING("M must be a valid mesh.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}


	///////////////////
	// COMPONENT'S WORK

	Variant outMesh = NMesh_ConvertToTriMesh(inMesh);


	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = outMesh;
}


