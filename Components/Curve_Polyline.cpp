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


#include "Curve_Polyline.h"

#include <assert.h>

#include "Polyline.h"

using namespace Urho3D;

String Curve_Polyline::iconTexture = "Textures/Icons/Curve_Polyline.png";

Curve_Polyline::Curve_Polyline(Context* context) :
	IoComponentBase(context, 2, 1)
{
	SetName("Polyline");
	SetFullName("Construct Polyline");
	SetDescription("Construct a polyline from a vertex list");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("Vertices");
	inputSlots_[0]->SetVariableName("V");
	inputSlots_[0]->SetDescription("List of coordinates of vertices");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);

	inputSlots_[1]->SetName("Close");
	inputSlots_[1]->SetVariableName("C");
	inputSlots_[1]->SetDescription("Close the curve");
	inputSlots_[1]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(false);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Polyline");
	outputSlots_[0]->SetVariableName("P");
	outputSlots_[0]->SetDescription("Constructed polyline");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_Polyline::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	if (inSolveInstance[0].GetType() != VariantType::VAR_VARIANTVECTOR) {
		URHO3D_LOGWARNING("V must be a list of points (Vector3).");
		outSolveInstance[0] = Variant();
		return;
	}
	VariantVector varVec = inSolveInstance[0].GetVariantVector();
	if (varVec.Size() < 2) {
		URHO3D_LOGWARNING("V must contain at least 2 points (Vector3).");
		outSolveInstance[0] = Variant();
		return;
	}
	for (unsigned i = 0; i < varVec.Size(); ++i) {
		if (varVec[i].GetType() != VariantType::VAR_VECTOR3) {
			URHO3D_LOGWARNING("V must list only points (Vector3).");
			outSolveInstance[0] = Variant();
			return;
		}
	}
	// Verify input slot 1
	if (inSolveInstance[1].GetType() != VariantType::VAR_BOOL) {
		URHO3D_LOGWARNING("C must be a boolean.");
		outSolveInstance[0] = Variant();
		return;
	}
	bool close = inSolveInstance[1].GetBool();

	VariantVector verts = inSolveInstance[0].GetVariantVector();

	if (close)
	{
		/*
		 * Subtle bug alert!
		 * If we do
		 *   verts.Push(verts[0]) that last Variant ends up not having
		 * type VAR_VECTOR3, even though verts[0] is a Variant that does have type VAR_VECTOR3.
		 * I don't know why this happens.
		 */
		Vector3 start = verts[0].GetVector3();
		if (start != verts[verts.Size() - 1].GetVector3())
		{
			verts.Push(start);
		}
	}

	outSolveInstance[0] = Polyline_Make(verts);
}