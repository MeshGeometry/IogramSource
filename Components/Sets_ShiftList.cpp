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


#include "Sets_ShiftList.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Sets_ShiftList::iconTexture = "Textures/Icons/Sets_ShiftList.png";

Sets_ShiftList::Sets_ShiftList(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("ListItem");
	SetFullName("List Item");
	SetDescription("Select items by index");
	SetGroup(IoComponentGroup::SETS);
	SetSubgroup("");

	inputSlots_[0]->SetName("List");
	inputSlots_[0]->SetVariableName("L");
	inputSlots_[0]->SetDescription("List of items");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTVECTOR);
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);

	inputSlots_[1]->SetName("Shift Length");
	inputSlots_[1]->SetVariableName("S");
	inputSlots_[1]->SetDescription("Number of indices to shift");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDefaultValue(Variant(0));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Wrap");
	inputSlots_[2]->SetVariableName("W");
	inputSlots_[2]->SetDescription("Wrap the indices to branch count");
	inputSlots_[2]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[2]->SetDefaultValue(true);
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Shifted List");
	outputSlots_[0]->SetVariableName("O");
	outputSlots_[0]->SetDescription("Selected list items");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);
}

void Sets_ShiftList::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////

	VariantVector L = inSolveInstance[0].GetVariantVector();
	int s = inSolveInstance[1].GetInt();
	bool wrap = inSolveInstance[2].GetBool();

	VariantVector O;
	for (unsigned i = 0; i < L.Size(); i++)
	{
		if (wrap)
		{
			O.Push(L[(i + s) % L.Size()]);
		}
		else
		{
			if (i + s < L.Size())
			{
				O.Push(L[i + s]);
			}
		}
	}

	outSolveInstance[0] = O;

	////////////////////////////////////////////////////////////
}