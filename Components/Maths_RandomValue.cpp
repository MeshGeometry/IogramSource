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


#include "Maths_RandomValue.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_RandomValue::iconTexture = "Textures/Icons/Maths_RandomValue.png";

Maths_RandomValue::Maths_RandomValue(Urho3D::Context* context) : IoComponentBase(context, 4, 1)
{
	SetName("RandomValue");
	SetFullName("Random Value");
	SetDescription("Generates a random float between min and max");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Min");
	inputSlots_[0]->SetVariableName("min");
	inputSlots_[0]->SetDescription("Start of range (default = 0)");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDefaultValue(0.0f);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Max");
	inputSlots_[1]->SetVariableName("max");
	inputSlots_[1]->SetDescription("end of range (default = 1)");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDefaultValue(1.0f);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Seed");
	inputSlots_[2]->SetVariableName("S");
	inputSlots_[2]->SetDescription("Seed for random number generator (int)");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDefaultValue(1);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("Number");
	inputSlots_[3]->SetVariableName("N");
	inputSlots_[3]->SetDescription("Number of random numbers to generate (int)");
	inputSlots_[3]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[3]->SetDefaultValue(1);
	inputSlots_[3]->DefaultSet();

	outputSlots_[0]->SetName("RnadomValue");
	outputSlots_[0]->SetVariableName("R");
	outputSlots_[0]->SetDescription("Random float between min and max");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

}

void Maths_RandomValue::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////
	float min = inSolveInstance[0].GetFloat();
	float max = inSolveInstance[1].GetFloat();
	int seed = inSolveInstance[2].GetInt();
	int number = inSolveInstance[3].GetInt();

	if (max < min)
	{
		URHO3D_LOGERROR("Max must be larger than min! ");
		return;
	}

	SetRandomSeed(seed);

	VariantVector randomList;

	for (int i = 0; i < number; ++i)
	{
		float random = Random(min, max);
		randomList.Push(random);
	}
	
	Variant out(randomList);
	outSolveInstance[0] = out;

	////////////////////////////////////////////////////////////
}
