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


#include "Curve_PolylineRevolve.h"

#include <assert.h>

#include "Polyline.h"
#include "Geomlib_PolylineRevolve.h"

using namespace Urho3D;

Urho3D::String Curve_PolylineRevolve::iconTexture = "Textures/Icons/Curve_PolylineRevolve.png";

Curve_PolylineRevolve::Curve_PolylineRevolve(Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("PolylineRevolve");
	SetFullName("Polyline Revolve");
	SetDescription("Creates a surface of revolution from a polyline and an axis");
	SetGroup(IoComponentGroup::CURVE);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Section");
	inputSlots_[0]->SetVariableName("S");
	inputSlots_[0]->SetDescription("Section polyline");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Axis");
	inputSlots_[1]->SetVariableName("A");
	inputSlots_[1]->SetDescription("Axis Vector");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(Vector3::ONE));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Resolution");
	inputSlots_[2]->SetVariableName("R");
	inputSlots_[2]->SetDescription("Mesh resolution");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(Variant(50));
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Mesh of revolution");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_PolylineRevolve::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	Variant sectionIn = inSolveInstance[0];
	if (!Polyline_Verify(sectionIn)) {
		URHO3D_LOGWARNING("Section must be a valid polyline.");
		outSolveInstance[0] = Variant();
		return;
	}
	// Verify input slot 1
	Variant axisIn = inSolveInstance[1];
	if (axisIn.GetType() != VAR_VECTOR3) {
		URHO3D_LOGWARNING("Axis must be a valid Vector3.");
		outSolveInstance[0] = Variant();
		return;
	}
	Vector3 axis = axisIn.GetVector3();

	// Verify input slot 2
	Variant resIn = inSolveInstance[2];
	int res = 50;
	if (resIn.GetType() != VAR_INT) {
		URHO3D_LOGWARNING("Invalid resolution, using res = 50.");
	}
	else
		res = resIn.GetInt();


	///////////////////
	// COMPONENT'S WORK

	Variant mesh;
	bool success = false;
	success = Geomlib::PolylineRevolve(axis, sectionIn, res, mesh);
	if (!success) {
		URHO3D_LOGWARNING("PolylineRevolve operation failed.");
		outSolveInstance[0] = Variant();
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = mesh;
}
