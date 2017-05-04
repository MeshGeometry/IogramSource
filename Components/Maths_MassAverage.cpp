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


#include "Maths_MassAverage.h"

#include <assert.h>

#include <iostream>

using namespace Urho3D;

String Maths_MassAverage::iconTexture = "Textures/Icons/Maths_MassAverage.png";

Maths_MassAverage::Maths_MassAverage(Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("MassAvg");
	SetFullName("Mass Average");
	SetDescription("Average of list of numbers");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("List");
	inputSlots_[0]->SetVariableName("L");
	inputSlots_[0]->SetDescription("List of floating point values");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);
	inputSlots_[0]->SetDefaultValue(Variant(0.0f));
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("Average");
	outputSlots_[0]->SetVariableName("A");
	outputSlots_[0]->SetDescription("Average of values");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Maths_MassAverage::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	Variant variant = inSolveInstance[0];
	VariantVector argsToAverage = variant.GetVariantVector();

	if (argsToAverage.Empty())
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	VariantType type = argsToAverage.At(0).GetType();
	Variant result(0.0f);

	if (type == VAR_FLOAT || type == VAR_DOUBLE || type == VAR_INT)
	{
		float tmpResult = 0.0f;
		for (unsigned i = 0; i < argsToAverage.Size(); ++i) {
			tmpResult += argsToAverage[i].GetFloat();
		}
		if (argsToAverage.Size() > 0) {
			result = tmpResult / argsToAverage.Size();
		}
	}
	else if (type == VAR_VECTOR3)
	{
		Vector3 tmpResult(0, 0, 0);
		for (unsigned i = 0; i < argsToAverage.Size(); ++i) {
			tmpResult += argsToAverage[i].GetVector3();
		}
		if (argsToAverage.Size() > 0) {
			result = tmpResult / argsToAverage.Size();
		}
	}


	/////////////////////////////////////////////////////////

	outSolveInstance[0] = result;
}
