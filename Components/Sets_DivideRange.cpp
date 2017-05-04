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


#include "Sets_DivideRange.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Sets_DivideRange::iconTexture = "Textures/Icons/Sets_DivideRange.png";

Sets_DivideRange::Sets_DivideRange(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("Divide");
	SetFullName("");
	SetDescription("Divides a range into N equal size chunks");
	SetGroup(IoComponentGroup::SETS);
	SetSubgroup("Sequence");

	inputSlots_[0]->SetName("Min");
	inputSlots_[0]->SetVariableName("A");
	inputSlots_[0]->SetDescription("Range minimum (float)");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDefaultValue(Variant(0.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Max");
	inputSlots_[1]->SetVariableName("B");
	inputSlots_[1]->SetDescription("Range maximum (float > minimum)");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDefaultValue(Variant(100.0f));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Number");
	inputSlots_[2]->SetVariableName("N");
	inputSlots_[2]->SetDescription("Number of partitions");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDefaultValue(Variant(10));
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Series");
	outputSlots_[0]->SetVariableName("S");
	outputSlots_[0]->SetDescription("Divided Range");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);
}

void Sets_DivideRange::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////
	// Verify input slot 0
	VariantType type0 = inSolveInstance[0].GetType();
	if (!(type0 == VariantType::VAR_FLOAT || type0 == VariantType::VAR_INT)) {
		URHO3D_LOGWARNING("Min must be a valid float or integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	float min = inSolveInstance[0].GetFloat();

	// Verify input slot 1
	VariantType type1 = inSolveInstance[1].GetType();
	if (!(type1 == VariantType::VAR_FLOAT || type1 == VariantType::VAR_INT)) {
		URHO3D_LOGWARNING("Max must be a valid float or integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	float max = inSolveInstance[1].GetFloat();
	if (max < min) {
		URHO3D_LOGWARNING("Max must be > min.");
		outSolveInstance[0] = Variant();
		return;
	}

	// Verify input slot 2
	int N = inSolveInstance[2].GetInt();
	if (N == 0) {
		URHO3D_LOGWARNING("N must not be zero");
		outSolveInstance[0] = Variant();
		return;
	}

	VariantVector seriesList;
	seriesList.Push(min);
	float sz = (max - min) / N;

	for (int i = 1; i < N+1; ++i) {
		seriesList.Push(min + i * sz);
	}
	Variant out(seriesList);
	outSolveInstance[0] = out;

	////////////////////////////////////////////////////////////
}
