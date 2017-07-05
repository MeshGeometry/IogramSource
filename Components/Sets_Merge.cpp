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


#include "Sets_Merge.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Sets_Merge::iconTexture = "Textures/Icons/Sets_Merge.png";

Sets_Merge::Sets_Merge(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("Merge");
	SetFullName("Merge");
	SetDescription("Merges two lists according to pattern");
	SetGroup(IoComponentGroup::SETS);
	SetSubgroup("");

	VariantVector defaultPattern;
	defaultPattern.Push(true);
	defaultPattern.Push(false);

	inputSlots_[0]->SetName("List_A");
	inputSlots_[0]->SetVariableName("A");
	inputSlots_[0]->SetDescription("First List");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTVECTOR);
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);

	inputSlots_[1]->SetName("List_B");
	inputSlots_[1]->SetVariableName("B");
	inputSlots_[1]->SetDescription("Second List");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTVECTOR);
	inputSlots_[1]->SetDataAccess(DataAccess::LIST);

	inputSlots_[2]->SetName("Pattern");
	inputSlots_[2]->SetVariableName("P");
	inputSlots_[2]->SetDescription("Pattern of separation");
	inputSlots_[2]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[2]->SetDefaultValue(Variant(defaultPattern));
	inputSlots_[2]->DefaultSet();
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("MergedList");
	outputSlots_[0]->SetVariableName("L");
	outputSlots_[0]->SetDescription("New compsed list");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);
}

void Sets_Merge::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////

	VariantVector list_A = inSolveInstance[0].GetVariantVector();
	VariantVector list_B = inSolveInstance[1].GetVariantVector();
	VariantVector P = inSolveInstance[2].GetVariantVector();

	VariantVector mergedList;

	//// set up a pattern vector of the appropriate length
	//Vector<bool> pattern;
	//int p_length = P.Size();
	//for (int i = 0; i < (list_A.Size() + list_B.Size()); ++i)
	//{
	//	pattern.Push(P[i % p_length].GetBool());
	//}

	// set up a pattern vector of the appropriate length
	// accepted inputs are vectors of bools or ints.
	Vector<bool> pattern;
	int p_length = P.Size();

	if (P.Empty())
	{
		outSolveInstance[0] = Variant();
		return;
	}
	
	for (unsigned i = 0; i < (list_A.Size() + list_B.Size()); ++i)
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

	// iterate through the pattern vector and merge appropriate list item
	int A_count = 0;
	int B_count = 0;
	int A_size = list_A.Size();
	int B_size = list_B.Size();
	for (unsigned i = 0; i < (list_A.Size() + list_B.Size()); ++i)
	{
		if (pattern[i] == true) {
			mergedList.Push(list_A[A_count % A_size]);
			A_count++;
		}
		else if (pattern[i] == false) {
			mergedList.Push(list_B[B_count % B_size]);
			B_count++;
		}
			
	}

	outSolveInstance[0] = Variant(mergedList);

	////////////////////////////////////////////////////////////
}