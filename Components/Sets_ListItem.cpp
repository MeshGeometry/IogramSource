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


#include "Sets_ListItem.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Sets_ListItem::iconTexture = "Textures/Icons/Sets_ListItem.png";

Sets_ListItem::Sets_ListItem(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
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

	inputSlots_[1]->SetName("Indices");
	inputSlots_[1]->SetVariableName("I");
	inputSlots_[1]->SetDescription("Indices of desired list items");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDefaultValue(Variant(0));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("List items");
	outputSlots_[0]->SetVariableName("O");
	outputSlots_[0]->SetDescription("Selected list items");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Sets_ListItem::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////

	if (inSolveInstance[0].GetType() == VAR_NONE ||
		inSolveInstance[1].GetType() == VAR_NONE)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	VariantVector L = inSolveInstance[0].GetVariantVector();
	int i = inSolveInstance[1].GetInt();

	Variant out_var;
	if (0 <= i && i < L.Size()) {
		out_var = L[i];
	}
	else {
		out_var = Variant();
	}

	outSolveInstance[0] = out_var;

	////////////////////////////////////////////////////////////
}
