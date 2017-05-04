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


#include "Maths_Multiplication.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_Multiplication::iconTexture = "Textures/Icons/Maths_Multiplication.png";

Maths_Multiplication::Maths_Multiplication(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("X * Y");
	SetFullName("Multiplication");
	SetDescription("Mathematical multiplication");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("X");
	inputSlots_[0]->SetVariableName("X");
	inputSlots_[0]->SetDescription("First number to multiply");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant(0.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Y");
	inputSlots_[1]->SetVariableName("Y");
	inputSlots_[1]->SetDescription("Second number to multiply");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(0.0f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Product");
	outputSlots_[0]->SetVariableName("P");
	outputSlots_[0]->SetDescription("Product of the numbers");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Maths_Multiplication::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	Variant factor0 = inSolveInstance[0];
	Variant factor1 = inSolveInstance[1];

	VariantType type0 = factor0.GetType();
	VariantType type1 = factor1.GetType();

	Variant product;

	////////////////////////////////////////////////////////////
	if (
		type0 == VariantType::VAR_FLOAT &&
		type1 == VariantType::VAR_FLOAT
		)
	{
		product = Variant(factor0.GetFloat() * factor1.GetFloat());
	}
	else if (
		type0 == VariantType::VAR_FLOAT &&
		type1 == VariantType::VAR_VECTOR3
		)
	{
		product = Variant(factor0.GetFloat() * factor1.GetVector3());
	}
	else if (
		type0 == VariantType::VAR_VECTOR3 &&
		type1 == VariantType::VAR_FLOAT
		)
	{
		product = Variant(factor1.GetFloat() * factor0.GetVector3());
	}
	else if (
		type0 == VariantType::VAR_MATRIX3X4 &&
		type1 == VariantType::VAR_MATRIX3X4
		)
	{
		product = Variant(factor1.GetMatrix3x4() * factor0.GetMatrix3x4());
	}
	else if (
		type0 == VariantType::VAR_INT &&
		type1 == VariantType::VAR_INT
		)
	{
		product = Variant(factor0.GetInt() * factor1.GetInt());
	}
	else if (
		type0 == VariantType::VAR_INT &&
		type1 == VariantType::VAR_FLOAT
		)
	{
		product = Variant(factor0.GetInt() * factor1.GetFloat());
	}
	else if (
		type0 == VariantType::VAR_FLOAT &&
		type1 == VariantType::VAR_INT
		)
	{
		product = Variant(factor0.GetFloat() * factor1.GetInt());
	}
	else if (
		type0 == VariantType::VAR_QUATERNION &&
		type1 == VariantType::VAR_QUATERNION
		)
	{
		product = Variant(factor0.GetQuaternion() * factor1.GetQuaternion());
	}
	else {
		product = Variant();
	}

	outSolveInstance[0] = product;
}
