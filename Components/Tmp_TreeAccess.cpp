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


#include "Tmp_TreeAccess.h"

#include <iostream>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Tmp_TreeAccess::iconTexture = "";

Tmp_TreeAccess::Tmp_TreeAccess(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("BoundingBox");
	SetFullName("Bounding Box");
	SetDescription("Construct TriMesh bounding box for another TriMesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Tree");
	inputSlots_[0]->SetVariableName("Tree");
	inputSlots_[0]->SetDescription("Tree");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::TREE);

	inputSlots_[1]->SetName("DummyIn");
	inputSlots_[1]->SetVariableName("DummyIn");
	inputSlots_[1]->SetDescription("DummyIn");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("DummyOut");
	outputSlots_[0]->SetVariableName("DummyOut");
	outputSlots_[0]->SetDescription("DummyOut");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Tmp_TreeAccess::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	float x = inSolveInstance[1].GetFloat();
	float y = 2 * x;

	VariantMap vm = inSolveInstance[0].GetVariantMap();

	Vector<StringHash> keys = vm.Keys();
	std::cout << "keys.Size()=" << keys.Size() << std::endl;

	for (unsigned i = 0; i < keys.Size(); ++i) {

		StringHash key = keys[i];
		std::cout << "key: " << key.ToString().CString() << std::endl;

		VariantVector vv = vm[key].GetVariantVector();
		for (int i = 0; i < vv.Size(); ++i) {
			std::cout << "val: " << vv[i].GetFloat() << std::endl;
		}
	}

	outSolveInstance[0] = y;
}