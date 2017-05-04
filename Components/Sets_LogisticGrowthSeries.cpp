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


#include "Sets_LogisticGrowthSeries.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Sets_LogisticGrowthSeries::iconTexture = "Textures/Icons/DefaultIcon.png";

Sets_LogisticGrowthSeries::Sets_LogisticGrowthSeries(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("LogisticGrowth");
	SetFullName("");
	SetDescription("Create a series of numbers following logistic growth curve");
	SetGroup(IoComponentGroup::SETS);
	SetSubgroup("Sequence");

	inputSlots_[0]->SetName("MaxVal");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Maximum Value in the series");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDefaultValue(Variant(1.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Steepness");
	inputSlots_[1]->SetVariableName("S");
	inputSlots_[1]->SetDescription("rate of increase");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDefaultValue(Variant(1.0f));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Count");
	inputSlots_[2]->SetVariableName("C");
	inputSlots_[2]->SetDescription("Number of values in the series");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDefaultValue(Variant(100));
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Series");
	outputSlots_[0]->SetVariableName("S");
	outputSlots_[0]->SetDescription("Series of numbers");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);
}

void Sets_LogisticGrowthSeries::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////
	float M = inSolveInstance[0].GetFloat();
	float S = inSolveInstance[1].GetFloat();
	int C = inSolveInstance[2].GetInt();

	VariantVector seriesList;
	for (int i = 0; i < C; ++i) {
		//seriesList.Push(S + i * N);

		float logisticGrowth = M / (1 + std::exp(-1* S *(i - C/2)));
		seriesList.Push(logisticGrowth);
	}
	Variant out(seriesList);
	outSolveInstance[0] = out;

	////////////////////////////////////////////////////////////
}
