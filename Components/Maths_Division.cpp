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


#include "Maths_Division.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_Division::iconTexture = "Textures/Icons/Maths_Division.png";

Maths_Division::Maths_Division(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("X/Y");
	SetFullName("Division");
	SetDescription("Mathematical division");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("X");
	inputSlots_[0]->SetVariableName("X");
	inputSlots_[0]->SetDescription("Number to divide");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant(1.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Y");
	inputSlots_[1]->SetVariableName("Y");
	inputSlots_[1]->SetDescription("Number to divide by");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(1.0f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Result");
	outputSlots_[0]->SetVariableName("R");
	outputSlots_[0]->SetDescription("Result of the division");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Maths_Division::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	Variant factor0 = inSolveInstance[0];
	Variant factor1 = inSolveInstance[1];

	VariantType type0 = factor0.GetType();
	VariantType type1 = factor1.GetType();

	Variant product;

	///////////////////
	// COMPONENT'S WORK

	if (
		type0 == VariantType::VAR_FLOAT &&
		type1 == VariantType::VAR_FLOAT
		)
	{
		// preserving original function logic in here
		float result = 1.0;
		if (inSolveInstance[1].GetFloat() == 0)
			result = M_INFINITY;
		else {
			result = inSolveInstance[0].GetFloat() / inSolveInstance[1].GetFloat();
		}
		product = Variant(result);
	}
	else if (
		type0 == VariantType::VAR_FLOAT &&
		type1 == VariantType::VAR_VECTOR3
		)
	{
		if (factor0.GetFloat() != 0.0f) {
			float scalar = 1.0f / factor0.GetFloat();
			product = Variant(scalar * factor1.GetVector3());
		}
		else {
			product = Variant();
		}
	}
	else if (
		type0 == VariantType::VAR_VECTOR3 &&
		type1 == VariantType::VAR_FLOAT
		)
	{
		if (factor1.GetFloat() != 0.0f) {
			float scalar = 1.0f / factor1.GetFloat();
			product = Variant(scalar * factor0.GetVector3());
		}
		else {
			product = Variant();
		}
	}
	else {
		product = Variant();
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = product;
}
