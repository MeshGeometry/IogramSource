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


#include "Curve_PolylineDivide.h"

#include <assert.h>

#include "Polyline.h"
#include "Geomlib_PolylineDivide.h"

using namespace Urho3D;

String Curve_PolylineDivide::iconTexture = "Textures/Icons/Curve_PolylineDivide.png";

Curve_PolylineDivide::Curve_PolylineDivide(Context* context) :
	IoComponentBase(context, 2, 1)
{
	SetName("DividePolyline");
	SetFullName("Divide Polyline");
	SetDescription("Divide polyline into equal parts");
	SetGroup(IoComponentGroup::CURVE);
	SetSubgroup("Geometry");

	inputSlots_[0]->SetName("Polyline");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Polyline to divide");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("NumParts");
	inputSlots_[1]->SetVariableName("N");
	inputSlots_[1]->SetDescription("Number of parts");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(10));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("DividedPolyline");
	outputSlots_[0]->SetVariableName("D");
	outputSlots_[0]->SetDescription("Polyline divided into parts");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_PolylineDivide::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	Variant polyline = inSolveInstance[0];
	if (!Polyline_Verify(polyline)) {
		URHO3D_LOGWARNING("P must be a valid polyline.");
		outSolveInstance[0] = Variant();
		return;
	}
	// Verify input slot 1
	if (inSolveInstance[1].GetType() != VariantType::VAR_INT) {
		URHO3D_LOGWARNING("N must be an integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	int numParts = inSolveInstance[1].GetInt();
	if (numParts < 1) {
		URHO3D_LOGWARNING("N must be >= 1.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Variant polylineOut;

	bool success = Geomlib::PolylineDivide(polyline, numParts, polylineOut);
	if (!success) {
		URHO3D_LOGWARNING("Polyline divide operation failed.");
		outSolveInstance[0] = Variant();
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = polylineOut;
}