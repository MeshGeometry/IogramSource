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


#include "Maths_GreaterThan.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_GreaterThan::iconTexture = "Textures/Icons/Maths_GreaterThan.png";

Maths_GreaterThan::Maths_GreaterThan(Urho3D::Context* context) : IoComponentBase(context, 2, 2)
{
	SetName("GreaterThan");
	SetFullName("Greater Than");
	SetDescription("Tests if input B is greater than input A");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("firstNumber");
	inputSlots_[0]->SetVariableName("A");
	inputSlots_[0]->SetDescription("First number");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDefaultValue(Variant(0.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("secondNumber");
	inputSlots_[1]->SetVariableName("B");
	inputSlots_[1]->SetDescription("Second number");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDefaultValue(Variant(0.0f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Greater than (>)");
	outputSlots_[0]->SetVariableName(">");
	outputSlots_[0]->SetDescription("True if A > B");
	outputSlots_[0]->SetVariantType(VariantType::VAR_BOOL);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Greater than or equal to (>=)");
	outputSlots_[1]->SetVariableName(">=");
	outputSlots_[1]->SetDescription("True if A >= B");
	outputSlots_[1]->SetVariantType(VariantType::VAR_BOOL);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Maths_GreaterThan::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////

	if (inSolveInstance[0].GetType() == VAR_NONE ||
		inSolveInstance[1].GetType() == VAR_NONE)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Variant lhs = inSolveInstance[0];
	Variant rhs = inSolveInstance[1];
	VariantType lhsType = lhs.GetType();
	VariantType rhsType = rhs.GetType();

	float A = inSolveInstance[0].GetFloat();
	float B = inSolveInstance[1].GetFloat();

	if (
		lhsType == VariantType::VAR_VECTOR3 &&
		rhsType == VariantType::VAR_VECTOR3
		)
	{
		A = lhs.GetVector3().Length();
		B = rhs.GetVector3().Length();
	}

	Variant greater_than;
	Variant greater_than_or_equal;

	if (A > B)
	{
		greater_than = true;
		greater_than_or_equal = true;
	}
	else if (A == B)
	{
		greater_than = false;
		greater_than_or_equal = true;
	}
	else
	{
		greater_than = false;
		greater_than_or_equal = false;
	}

	outSolveInstance[0] = greater_than;
	outSolveInstance[1] = greater_than_or_equal;

	////////////////////////////////////////////////////////////
}
