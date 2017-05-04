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


#include "Sets_VariantMap.h"

#include "Urho3D/Graphics/Technique.h"

using namespace Urho3D;

String Sets_VariantMap::iconTexture = "Textures/Icons/Sets_VariantMap.png";

Sets_VariantMap::Sets_VariantMap(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("VariantMap");
	SetFullName("Variant Map");
	SetDescription("Construct a variant map from keys and values");
	SetGroup(IoComponentGroup::PARAMS);
	SetSubgroup("");

	inputSlots_[0]->SetName("Key");
	inputSlots_[0]->SetVariableName("K");
	inputSlots_[0]->SetDescription("Key");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);

	inputSlots_[1]->SetName("Value");
	inputSlots_[1]->SetVariableName("V");
	inputSlots_[1]->SetDescription("Value");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDataAccess(DataAccess::LIST);

	outputSlots_[0]->SetName("Map");
	outputSlots_[0]->SetVariableName("VM");
	outputSlots_[0]->SetDescription("Variant Map");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}


void Sets_VariantMap::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	VariantMap map;
	VariantVector keyList = inSolveInstance[0].GetVariantVector();
	VariantVector valueList = inSolveInstance[1].GetVariantVector();

	int numItems = Min(keyList.Size(), valueList.Size());

	for (int i = 0; i < numItems; i++)
	{
		if (keyList[i].GetType() != VariantType::VAR_STRING)
		{
			URHO3D_LOGERROR("Must have string has key");
			return;
		}

		map[keyList[i].GetString()] = valueList[i];
	}

	outSolveInstance[0] = map;
}