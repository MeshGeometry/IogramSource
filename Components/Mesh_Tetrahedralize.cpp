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


#include "Mesh_Tetrahedralize.h"
#include <Urho3D/Core/Variant.h>
#include "Geomlib_MeshTetrahedralize.h"
#include "ConversionUtilities.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_Tetrahedralize::iconTexture = "Textures/Icons/Mesh_Tetrahedralize.png";

Mesh_Tetrahedralize::Mesh_Tetrahedralize(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("Tetrahedralize");
	SetFullName("Tetrahedralize Mesh");
	SetDescription("Create a tetrahedralization of a mesh");

	AddInputSlot(
		"Mesh",
		"M",
		"Mesh to tetrahedralize",
		VAR_VARIANTMAP,
		ITEM
		);

	AddInputSlot(
		"Volume",
		"V",
		"Optional volume condition",
		VAR_FLOAT,
		ITEM,
		-1.0f
		);

	AddOutputSlot(
		"MeshOut",
		"TM",
		"Tetrahedralized mesh out",
		VAR_VARIANTMAP,
		ITEM
		);

}

void Mesh_Tetrahedralize::SolveInstance(
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

	if (!TriMesh_Verify(inMesh)) {
		URHO3D_LOGWARNING("M must be a valid mesh.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	float volume = inSolveInstance[1].GetFloat();


	///////////////////
	// COMPONENT'S WORK

	Variant outMesh;
	bool success = Geomlib::MeshTetrahedralize(inMesh, volume, outMesh);
	if (!success) {
		URHO3D_LOGWARNING("Tetrahedralize operation failed.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = outMesh;
}