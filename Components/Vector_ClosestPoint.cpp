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


#include "Vector_ClosestPoint.h"

#include <assert.h>

#include <Urho3D/Math/Vector3.h>

#include <Eigen/Core>

using namespace Urho3D;

String Vector_ClosestPoint::iconTexture = "Textures/Icons/Vector_ClosestPoint.png";

Vector_ClosestPoint::Vector_ClosestPoint(Context* context) : IoComponentBase(context, 2, 3)
{
	SetName("ClosePoint");
	SetFullName("Closest Point");
	SetDescription("Which point in list is closest");
	SetGroup(IoComponentGroup::VECTOR);
	SetSubgroup("Point");

	inputSlots_[0]->SetName("Point");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Query point");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDefaultValue(Vector3(0.0f, 0.0f, 0.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Point List");
	inputSlots_[1]->SetVariableName("L");
	inputSlots_[1]->SetDescription("List of points");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDataAccess(DataAccess::LIST);
	inputSlots_[1]->SetDefaultValue(Vector3(0.0f, 0.0f, 0.0f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Closest Point");
	outputSlots_[0]->SetVariableName("C");
	outputSlots_[0]->SetDescription("Closest point in list to P");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);

	outputSlots_[1]->SetName("Index");
	outputSlots_[1]->SetVariableName("I");
	outputSlots_[1]->SetDescription("Index into L of closest point");
	outputSlots_[1]->SetVariantType(VariantType::VAR_INT);

	outputSlots_[2]->SetName("Distance");
	outputSlots_[2]->SetVariableName("D");
	outputSlots_[2]->SetDescription("Distance to closest point");
	outputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
}

void Vector_ClosestPoint::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	///////////////////
	// EXTRACT & VERIFY

	// query point
	Variant queryPointVar = inSolveInstance[0];
	if (queryPointVar.GetType() != VariantType::VAR_VECTOR3) {
		URHO3D_LOGWARNING("P must be a valid Vector3");
		outSolveInstance[0] = Variant();
		outSolveInstance[1] = Variant();
		outSolveInstance[2] = Variant();
		return;
	}
	Vector3 q = queryPointVar.GetVector3();

	// point list
	Variant pointListVar = inSolveInstance[1];
	if (pointListVar.GetType() != VariantType::VAR_VARIANTVECTOR) {
		URHO3D_LOGWARNING("L must be a list of Vector3's");
		outSolveInstance[0] = Variant();
		outSolveInstance[1] = Variant();
		outSolveInstance[2] = Variant();
		return;
	}
	Vector<Variant> pointList = pointListVar.GetVariantVector();

	//////////////////////////////////////////////
	// COMPONENT'S WORK & VERIFICATION DURING LOOP

	// output holders needed now to store data as we loop
	int index = -1; // index of closest point
	Vector3 r; // closest point
	float distance = -1.0f;

	/* due to the nature of this computation, we unpack and compute at the same time */
	if (pointList.Size() > 0) {

		Variant pointVar = pointList[0];
		if (pointVar.GetType() != VariantType::VAR_VECTOR3) {
			URHO3D_LOGWARNING("L must be a list of Vector3's only");
			outSolveInstance[0] = Variant();
			outSolveInstance[1] = Variant();
			outSolveInstance[2] = Variant();
			return;
		}
		Vector3 p = pointVar.GetVector3();
		float minDistanceSquared = (p - q).LengthSquared();
		index = 0;
		r = p;

		for (unsigned i = 1; i < pointList.Size(); ++i) {
			pointVar = pointList[i];
			if (pointVar.GetType() != VariantType::VAR_VECTOR3) {
				URHO3D_LOGWARNING("L must be a list of Vector3's only");
				outSolveInstance[0] = Variant();
				outSolveInstance[1] = Variant();
				outSolveInstance[2] = Variant();
				return;
			}
			p = pointVar.GetVector3();
			float distanceSquared = (p - q).LengthSquared();
			if (distanceSquared < minDistanceSquared) {
				minDistanceSquared = distanceSquared;
				index = i;
				r = p;
			}
		}

		distance = sqrt(minDistanceSquared);
	}

	/////////////////
	// ASSIGN OUTPUTS

	if (index > -1) {
		// inputs were good
		outSolveInstance[0] = Variant(r);
		outSolveInstance[1] = Variant((int)index);
		outSolveInstance[2] = Variant(distance);
	}
	else {
		// inputs were bad -- null sets?
		outSolveInstance[0] = Variant();
		outSolveInstance[1] = Variant();
		outSolveInstance[2] = Variant();
	}
}