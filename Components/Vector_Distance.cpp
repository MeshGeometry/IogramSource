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


#include "Vector_Distance.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>

using namespace Urho3D;

String Vector_Distance::iconTexture = "Textures/Icons/Vector_Distance.png";

Vector_Distance::Vector_Distance(Context* context) :
	IoComponentBase(context, 2, 1)
{
	SetName("Distance");
	SetFullName("Vector Distance");
	SetDescription("Compute distance between vectors");
	SetGroup(IoComponentGroup::VECTOR);
	SetSubgroup("Vector");

	inputSlots_[0]->SetName("Vector");
	inputSlots_[0]->SetVariableName("V");
	inputSlots_[0]->SetDescription("First vector");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Vector3(0.0f, 0.0f, 0.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Vector");
	inputSlots_[1]->SetVariableName("W");
	inputSlots_[1]->SetDescription("Second vector");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Vector3(0.0f, 0.0f, 0.0f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Distance");
	outputSlots_[0]->SetVariableName("D");
	outputSlots_[0]->SetDescription("Distance between vectors");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Vector_Distance::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	///////////////////
	// EXTRACT & VERIFY

	Variant varV = inSolveInstance[0];
	Variant varW = inSolveInstance[1];

	if (!(varV.GetType() == VariantType::VAR_VECTOR3)) {
		URHO3D_LOGWARNING("V must be a valid Vector3");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	Vector3 vecV = varV.GetVector3();

	if (!(varW.GetType() == VariantType::VAR_VECTOR3)) {
		URHO3D_LOGWARNING("W must be a valid Vector3");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	Vector3 vecW = varW.GetVector3();

	///////////////////
	// COMPONENT'S WORK

	float distanceSquared = (vecV - vecW).LengthSquared();
	float distance = sqrt(distanceSquared);

	////////////////
	// ASSIGN OUTPUT

	outSolveInstance[0] = Variant(distance);
}