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


#include "Curve_PolylineEvaluate.h"

#include <assert.h>

#include "Polyline.h"
#include "Geomlib_PolylinePointFromParameter.h"

using namespace Urho3D;

String Curve_PolylineEvaluate::iconTexture = "Textures/Icons/Curve_EvaluatePolyline.png";

Curve_PolylineEvaluate::Curve_PolylineEvaluate(Context* context) :
	IoComponentBase(context, 2, 2)
{
	SetName("EvaluatePolyline");
	SetFullName("Evaluate Polyline");
	SetDescription("Evaluate point on polyline from parameter");
	SetGroup(IoComponentGroup::CURVE);
	SetSubgroup("Geometry");

	inputSlots_[0]->SetName("Polyline");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Polyline");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Parameter");
	inputSlots_[1]->SetVariableName("T");
	inputSlots_[1]->SetDescription("Parameter to evaluate");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(0.5f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Point");
	outputSlots_[0]->SetVariableName("P");
	outputSlots_[0]->SetDescription("Point on polyline corresponding to parameter");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Transform");
	outputSlots_[1]->SetVariableName("T");
	outputSlots_[1]->SetDescription("Transform on polyline corresponding to parameter");
	outputSlots_[1]->SetVariantType(VariantType::VAR_MATRIX3X4);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Curve_PolylineEvaluate::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	if (!Polyline_Verify(inSolveInstance[0])) {
		URHO3D_LOGWARNING("P must be a valid polyline.");
		outSolveInstance[0] = Variant();
		return;
	}
	Variant polyline = inSolveInstance[0];
	// Verify input slot 1
	if (inSolveInstance[1].GetType() != VariantType::VAR_FLOAT) {
		URHO3D_LOGWARNING("T must have type float.");
		outSolveInstance[0] = Variant();
		return;
	}
	float t = inSolveInstance[1].GetFloat();
	if (t < 0.0f || t > 1.0f) {
		URHO3D_LOGWARNING("T must be in range 0.0f <= T <= 1.0f.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Vector3 point;
	Urho3D::Matrix3x4 transform;
	bool success = Geomlib::PolylinePointFromParameter(polyline, t, point);
	bool success_transform = Geomlib::PolylineTransformFromParameter(polyline, t, transform);



	if (!success || !success_transform) {
		URHO3D_LOGWARNING("EvaluatePolyline operation failed.");
		outSolveInstance[0] = Variant();
		outSolveInstance[1] = Variant();
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = Variant(point);
	outSolveInstance[1] = Variant(transform);
}