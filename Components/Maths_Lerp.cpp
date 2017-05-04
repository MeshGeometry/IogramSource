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


#include "Maths_Lerp.h"
#include "IoGraph.h"

using namespace Urho3D;

String Maths_Lerp::iconTexture = "Textures/Icons/Maths_Lerp.png";

Maths_Lerp::Maths_Lerp(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("LinInterp");
	SetFullName("Linear Interpolation");
	SetDescription("Linear interpolation between two values");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("");


	inputSlots_[0]->SetName("From");
	inputSlots_[0]->SetVariableName("A");
	inputSlots_[0]->SetDescription("Value to lerp from");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(0.0f);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("To");
	inputSlots_[1]->SetVariableName("B");
	inputSlots_[1]->SetDescription("Value to lerp to");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(1.0f);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("T");
	inputSlots_[2]->SetVariableName("t");
	inputSlots_[2]->SetDescription("Parameter t");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(0.5f);
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Lerped Value");
	outputSlots_[0]->SetVariableName("V");
	outputSlots_[0]->SetDescription("Value at param t");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}


void Maths_Lerp::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	if (inSolveInstance[0].GetType() != inSolveInstance[1].GetType())
	{
		URHO3D_LOGERROR("Can't lerp values of different types.");
		outSolveInstance[0] = 0.0f;
		return;
	}

	float t = inSolveInstance[2].GetFloat();

	if (inSolveInstance[0].GetType() == VAR_FLOAT || inSolveInstance[0].GetType() == VAR_INT)
	{
		float a = inSolveInstance[0].GetFloat();
		float b = inSolveInstance[1].GetFloat();

		outSolveInstance[0] = Lerp(a, b, t);
		return;
	}

	if (inSolveInstance[0].GetType() == VAR_VECTOR3)
	{
		Vector3 a = inSolveInstance[0].GetVector3();
		Vector3 b = inSolveInstance[1].GetVector3();

		outSolveInstance[0] = Lerp(a, b, t);
		return;
	}

	if (inSolveInstance[0].GetType() == VAR_COLOR)
	{
		Color a = inSolveInstance[0].GetColor();
		Color b = inSolveInstance[1].GetColor();

		outSolveInstance[0] = Lerp(a, b, t);
		return;
	}

	if (inSolveInstance[0].GetType() == VAR_QUATERNION)
	{
		Quaternion a = inSolveInstance[0].GetQuaternion();
		Quaternion b = inSolveInstance[1].GetQuaternion();

		outSolveInstance[0] = Lerp(a, b, t);
		return;
	}
}
