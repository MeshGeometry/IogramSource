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


#include "Curve_SelfIntersections.h"

#include <assert.h>

#include "Polyline.h"
#include "Geomlib_PolylineIntersection.h"

using namespace Urho3D;

String Curve_SelfIntersections::iconTexture = "";


Curve_SelfIntersections::Curve_SelfIntersections(Context* context) : IoComponentBase(context, 1, 2)
{
	SetName("FindSelfIntersections");
	SetFullName("Self Intersections");
	SetDescription("Finds Self Intersections of a Planar Polyline");
	SetGroup(IoComponentGroup::CURVE);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Polyline");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Polyline to analyze");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("Intersections");
	outputSlots_[0]->SetVariableName("I");
	outputSlots_[0]->SetDescription("Points of Intersection");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[1]->SetName("Polyline");
	outputSlots_[1]->SetVariableName("P");
	outputSlots_[1]->SetDescription("Polyline with Intersections added");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Curve_SelfIntersections::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	if (inSolveInstance[0].GetType() != VariantType::VAR_VARIANTMAP) {
		URHO3D_LOGWARNING("P must be a polyline.");
		outSolveInstance[0] = Variant();
		return;
	}
	VariantMap polyline_in = inSolveInstance[0].GetVariantMap();
	if (!Polyline_Verify(polyline_in))
	{
		URHO3D_LOGWARNING("P must be a polyline.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	bool success = Geomlib::HasSelfIntersection(inSolveInstance[0]);
	Vector<Vector3> intersections;
	Variant revised_polyline;
	int res = Geomlib::GetSelfIntersections(inSolveInstance[0], intersections, revised_polyline);

	VariantVector intersections_var;
	for (int i = 0; i < intersections.Size(); ++i)
		intersections_var.Push(Variant(intersections[i]));


	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = intersections_var;
	outSolveInstance[1] = revised_polyline;
}



