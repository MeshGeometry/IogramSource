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


#include "Curve_SmoothPolyline.h"

#include <assert.h>

#include "Polyline.h"
#include "Geomlib_SmoothPolyline.h"

using namespace Urho3D;

String Curve_SmoothPolyline::iconTexture = "Textures/Icons/Curve_SmoothPolyline.png";

Curve_SmoothPolyline::Curve_SmoothPolyline(Context* context) :
	IoComponentBase(context, 2, 1)
{
	SetName("SmoothPolyline");
	SetFullName("Smooth Polyline");
	SetDescription("Smooth polyline via subdivision");
	SetGroup(IoComponentGroup::CURVE);
	SetSubgroup("Geometry");

	inputSlots_[0]->SetName("Polyline");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Polyline to smooth");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Iterations");
	inputSlots_[1]->SetVariableName("I");
	inputSlots_[1]->SetDescription("Number of smoothing iterations to perform");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(1));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Smoothed Polyline");
	outputSlots_[0]->SetVariableName("P");
	outputSlots_[0]->SetDescription("Smoothed and subdivided polyline");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_SmoothPolyline::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	Variant inPolyline = inSolveInstance[0];
	if (!Polyline_Verify(inPolyline)) {
		URHO3D_LOGWARNING("P must be a valid polyline.");
		outSolveInstance[0] = Variant();
		return;
	}
	// Verify input slot 1
	if (inSolveInstance[1].GetType() != VariantType::VAR_INT) {
		URHO3D_LOGWARNING("I must be an integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	int steps = inSolveInstance[1].GetInt();
	if (steps < 0) {
		URHO3D_LOGWARNING("I must be >= 0.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Variant outPolyline;

	if (steps == 0) {
		outPolyline = inPolyline;
	}
	else {
		for (int i = 0; i < steps; ++i) {
			bool success = Geomlib::SmoothPolyline(inPolyline, outPolyline);
			if (!success) {
				URHO3D_LOGWARNING("Smoothing operation failed.");
				outSolveInstance[0] = Variant();
				return;
			}
			inPolyline = outPolyline;
		}
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = outPolyline;
}