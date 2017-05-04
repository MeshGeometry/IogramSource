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


#include "Curve_PolylineRefine.h"

#include <assert.h>

#include "Geomlib_PolylineRefine.h"

using namespace Urho3D;

String Curve_PolylineRefine::iconTexture = "";

Curve_PolylineRefine::Curve_PolylineRefine(Context* context) :
	IoComponentBase(context, 2, 1)
{
	SetName("RefinePolyline");
	SetFullName("Refine Polyline");
	SetDescription("Refine polyline based on list of parameters");
	SetGroup(IoComponentGroup::CURVE);
	SetSubgroup("Geometry");

	inputSlots_[0]->SetName("Polyline");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Polyline to refine");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Params");
	inputSlots_[1]->SetVariableName("T");
	inputSlots_[1]->SetDescription("List of parameters");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::LIST);
	inputSlots_[1]->SetDefaultValue(Variant(0.33f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("RefinedPolyline");
	outputSlots_[0]->SetVariableName("R");
	outputSlots_[0]->SetDescription("Polyline refined by listed parameters");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_PolylineRefine::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	Variant polyline = inSolveInstance[0];
	Variant paramVar = inSolveInstance[1];
	paramVar.GetType();

	Variant polylineOut;

	if (paramVar.GetType() != VariantType::VAR_VARIANTVECTOR) {
		// paramVar isn't correct format, just pass the polyline thru unrefined
		polylineOut = polyline;
	}
	else {
		VariantVector paramVec = paramVar.GetVariantVector();
		Vector<float> params;
		for (unsigned i = 0; i < paramVec.Size(); ++i) {
			if (paramVec[i].GetType() != VariantType::VAR_FLOAT) {
				// skip whatever is stored in the Variant paramVec[i]
			}
			else {
				params.Push(paramVec[i].GetFloat());
			}
		}
		Vector<float> cleanParams;
		for (unsigned i = 0; i < params.Size(); ++i) {
			if (0.0f < params[i] && params[i] < 1.0f) {
				cleanParams.Push(params[i]);
			}
		}
		bool success = Geomlib::PolylineRefine(polyline, cleanParams, polylineOut);
	}

	outSolveInstance[0] = polylineOut;
}