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


#include "Maths_Addition.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_Addition::iconTexture = "Textures/Icons/Maths_Addition.png";
String Maths_Addition::tags = "All";

Maths_Addition::Maths_Addition(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("X + Y");
	SetFullName("Addition");
	SetDescription("Mathematical addition");

	inputSlots_[0]->SetName("X");
	inputSlots_[0]->SetVariableName("X");
	inputSlots_[0]->SetDescription("First number to add");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant(0.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Y");
	inputSlots_[1]->SetVariableName("Y");
	inputSlots_[1]->SetDescription("Second number to add");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(0.0f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Sum");
	outputSlots_[0]->SetVariableName("S");
	outputSlots_[0]->SetDescription("Sum of the numbers");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}


//main functionality
void Maths_Addition::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	//////////
	// EXTRACT

	Variant addend0 = inSolveInstance[0];
	Variant addend1 = inSolveInstance[1];

	VariantType type0 = addend0.GetType();
	VariantType type1 = addend1.GetType();

	Variant result;

	////////////////////////////
	// VERIFY & COMPONENT'S WORK

	if (
		type0 == VariantType::VAR_VECTOR3 &&
		type1 == VariantType::VAR_VECTOR3
		)
	{
		Vector3 v = addend0.GetVector3() + addend1.GetVector3();
		result = Variant(v);
	}
	else if (
		(type0 == VariantType::VAR_INT || type0 == VariantType::VAR_FLOAT) &&
		(type1 == VariantType::VAR_INT || type1 == VariantType::VAR_FLOAT)
		)
	{
		float s = addend0.GetFloat() + addend1.GetFloat();
		result = Variant(s);
	}
	else if (
		type0 == VariantType::VAR_MATRIX3X4 &&
		type1 == VariantType::VAR_MATRIX3X4
		)
	{
		result = Variant(addend0.GetMatrix3x4() + addend1.GetMatrix3x4());
	}
	else if (
		type0 == VariantType::VAR_QUATERNION &&
		type1 == VariantType::VAR_QUATERNION
		)
	{
		result = Variant(addend0.GetQuaternion() + addend1.GetQuaternion());
	}
	else
	{
		URHO3D_LOGWARNING("X and Y must both be valid Vector3's or valid numbers.");
		result = Variant();
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = result;
}
