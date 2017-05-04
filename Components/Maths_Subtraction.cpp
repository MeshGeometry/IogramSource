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


#include "Maths_Subtraction.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_Subtraction::iconTexture = "Textures/Icons/Maths_Subtraction.png";

Maths_Subtraction::Maths_Subtraction(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("X - Y");
	SetFullName("Subtraction");
	SetDescription("Mathematical subtraction");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("X");
	inputSlots_[0]->SetVariableName("X");
	inputSlots_[0]->SetDescription("Number to subtract from");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant(0.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Y");
	inputSlots_[1]->SetVariableName("Y");
	inputSlots_[1]->SetDescription("Number to subtract");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(0.0f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Result");
	outputSlots_[0]->SetVariableName("R");
	outputSlots_[0]->SetDescription("Result of the subtraction");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Maths_Subtraction::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	Urho3D::Variant minuend = inSolveInstance[0];
	Urho3D::Variant subtrahend = inSolveInstance[1];

	Urho3D::VariantType minuendType = minuend.GetType();
	Urho3D::VariantType subtrahendType = subtrahend.GetType();

	Variant difference;

	////////////////////////////////////////////////////////////
	if (
		minuendType == VariantType::VAR_VECTOR3 &&
		subtrahendType == VariantType::VAR_VECTOR3
		)
	{
		Vector3 v = minuend.GetVector3() - subtrahend.GetVector3();
		difference = Variant(v);
	}
	else if (
		(minuendType == VariantType::VAR_INT || minuendType == VariantType::VAR_FLOAT) &&
		(subtrahendType == VariantType::VAR_INT || subtrahendType == VariantType::VAR_FLOAT)
		)
	{
		float s = minuend.GetFloat() - subtrahend.GetFloat();
		difference = Variant(s);
	}
	else if (
		minuendType == VariantType::VAR_MATRIX3X4 &&
		subtrahendType == VariantType::VAR_MATRIX3X4
		)
	{
		difference = Variant(minuend.GetMatrix3x4() - subtrahend.GetMatrix3x4());
	}
	else if (
		minuendType == VariantType::VAR_QUATERNION &&
		subtrahendType == VariantType::VAR_QUATERNION
		)
	{
		difference = Variant(minuend.GetQuaternion() + subtrahend.GetQuaternion());
	}
	else
	{
		difference = Variant();
	}
	////////////////////////////////////////////////////////////

	outSolveInstance[0] = difference;

}
