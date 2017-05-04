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


#include "Sets_ListConstruct.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Sets_ListConstruct::iconTexture = "Textures/Icons/Sets_ListConstruct.png";

Sets_ListConstruct::Sets_ListConstruct(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("ListConstruct");
	SetFullName("List Construct");
	SetDescription("Construct a list from items");
	SetGroup(IoComponentGroup::SETS);
	SetSubgroup("LIST");

	inputSlots_[0]->SetName("Item1");
	inputSlots_[0]->SetVariableName("I1");
	inputSlots_[0]->SetDescription("First item");
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);

	inputSlots_[1]->SetName("Item2");
	inputSlots_[1]->SetVariableName("I2");
	inputSlots_[1]->SetDescription("Second item");
	inputSlots_[1]->SetDataAccess(DataAccess::LIST);

	outputSlots_[0]->SetName("List");
	outputSlots_[0]->SetVariableName("L");
	outputSlots_[0]->SetDescription("List of items");
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);
}

void Sets_ListConstruct::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////

	VariantVector theList;

	for (int i = 0; i < inSolveInstance.Size(); ++i) {
		Variant var = inSolveInstance[i];
		if (var.GetType() != VariantType::VAR_NONE) {

			if (var.GetType() == VariantType::VAR_VARIANTVECTOR) {

				VariantVector cur_list = var.GetVariantVector();
				for (int j = 0; j < cur_list.Size(); ++j) {
					if (cur_list[j].GetType() != VAR_NONE)
						theList.Push(cur_list[j]);
				}
			}
			else {
				theList.Push(var);
			}
		}
	}

	Variant out_var(theList);

	outSolveInstance[0] = out_var;

	////////////////////////////////////////////////////////////
}
