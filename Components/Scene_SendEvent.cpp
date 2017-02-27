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


#include "Scene_SendEvent.h"

#include <assert.h>

using namespace Urho3D;

String Scene_SendEvent::iconTexture = "Textures/Icons/Scene_SendEvent.png";

Scene_SendEvent::Scene_SendEvent(Context* context) :
	IoComponentBase(context, 2, 0)
{
	SetName("SendEvent");
	SetFullName("Send Event");
	SetDescription("Define and send an event");

	inputSlots_[0]->SetName("EventName");
	inputSlots_[0]->SetVariableName("E");
	inputSlots_[0]->SetDescription("Unique event name to send");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("DataTree");
	inputSlots_[1]->SetVariableName("T");
	inputSlots_[1]->SetDescription("DataTree to send");
	inputSlots_[1]->SetVariantType(VariantType::VAR_NONE);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}


int Scene_SendEvent::LocalSolve()
{
	assert(inputSlots_.Size() >= 2);
	assert(outputSlots_.Size() == 0);

	if (inputSlots_[0]->HasNoData() || inputSlots_[1]->HasNoData()) {
		solvedFlag_ = 0;
		return 0;
	}

	IoDataTree nameTree = inputSlots_[0]->GetIoDataTree();
	SharedPtr<IoDataTree> inputIoDataTree = SharedPtr<IoDataTree>(new IoDataTree(inputSlots_[1]->GetIoDataTree()));

	Vector<int> path = nameTree.Begin();
	Variant name;
	nameTree.GetItem(name, path, 0);

	String eventName = name.GetString();
	if (eventName.Empty())
	{
		solvedFlag_ = 0;
		return 0;
	}

	VariantMap data;
	data["DataTree"] = inputIoDataTree;

	SendEvent(eventName, data);

	solvedFlag_ = 1;
	return 1;
}
