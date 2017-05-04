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


#include "Sets_IfThen.h"

#include <iostream>

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Sets_IfThen::iconTexture = "Textures/Icons/Sets_IfThen.png";

Sets_IfThen::Sets_IfThen(Urho3D::Context* context) : IoComponentBase(context, 2, 2)
{
	SetName("IfThen");
	SetFullName("If then");
	SetDescription("Separates list items according to pattern");
	SetGroup(IoComponentGroup::SETS);
	SetSubgroup("");

	VariantVector defaultPattern;
	defaultPattern.Push(true);
	defaultPattern.Push(false);

	inputSlots_[0]->SetName("List");
	inputSlots_[0]->SetVariableName("L");
	inputSlots_[0]->SetDescription("List of items");
	inputSlots_[0]->SetVariantType(VariantType::VAR_NONE); // originally VAR_VARIANTVECTOR
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);

	inputSlots_[1]->SetName("Pattern");
	inputSlots_[1]->SetVariableName("P");
	inputSlots_[1]->SetDescription("Pattern of separation");
	inputSlots_[1]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[1]->SetDefaultValue(Variant(defaultPattern));
	inputSlots_[1]->DefaultSet();
	inputSlots_[1]->SetDataAccess(DataAccess::LIST);

	outputSlots_[0]->SetName("List A");
	outputSlots_[0]->SetVariableName("T");
	outputSlots_[0]->SetDescription("'true' list items");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[1]->SetName("List B");
	outputSlots_[1]->SetVariableName("F");
	outputSlots_[1]->SetDescription("'false list items");
	outputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[1]->SetDataAccess(DataAccess::LIST);
}

void Sets_IfThen::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////

	VariantVector L = inSolveInstance[0].GetVariantVector();
	VariantVector P = inSolveInstance[1].GetVariantVector();

	VariantVector list_true;
	VariantVector list_false;
	
	// set up a pattern vector of the appropriate length
	// accepted inputs are vectors of bools or ints.
	Vector<bool> pattern;
	int p_length = P.Size();
	if (p_length == 0) {
		URHO3D_LOGWARNING("Sets_IfThen --- P must be a non-empty list of boolean values!");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	for (int i = 0; i < L.Size(); ++i)
	{
		VariantType curType = P[i % p_length].GetType();
		if (curType == VAR_BOOL)
			pattern.Push(P[i % p_length].GetBool());
		else if (curType == VAR_INT || curType == VAR_FLOAT) {
			int toConvert = P[i % p_length].GetInt();
			if (toConvert == 1)
				pattern.Push(true);
			else
				pattern.Push(false);
		}
		else
			pattern.Push(false);
	}
	if (pattern.Size() != L.Size()) {
		URHO3D_LOGWARNING("Sets_IfThen --- unable to setup equal length for value list and pattern list");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	// iterate through the list and separate according to pattern vector
	for (int i = 0; i < L.Size(); ++i)
	{
		if (pattern[i] == true)
			list_true.Push(L[i]);
		else if (pattern[i] == false)
			list_false.Push(L[i]);
	}

	outSolveInstance[0] = Variant(list_true);
	outSolveInstance[1] = Variant(list_false);

	////////////////////////////////////////////////////////////
}
