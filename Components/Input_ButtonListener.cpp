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


#include "Input_ButtonListener.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>

#include "IoGraph.h"

using namespace Urho3D;

String Input_ButtonListener::iconTexture = "Textures/Icons/Input_ToggleListener.png";


Input_ButtonListener::Input_ButtonListener(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("ButtonListener");
	SetFullName("Button Listener");
	SetDescription("Listens to button in scene");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("INPUT");

	inputSlots_[0]->SetName("Button Pointers");
	inputSlots_[0]->SetVariableName("BP");
	inputSlots_[0]->SetDescription("Pointers to screen buttons");
	inputSlots_[0]->SetVariantType(VariantType::VAR_PTR);

	inputSlots_[1]->SetName("Mute");
	inputSlots_[1]->SetVariableName("M");
	inputSlots_[1]->SetDescription("Mute this listener");
	inputSlots_[1]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[1]->SetDefaultValue(false);
	inputSlots_[1]->DefaultSet();


	outputSlots_[0]->SetName("Values");
	outputSlots_[0]->SetVariableName("V");
	outputSlots_[0]->SetDescription("Values Out");
	outputSlots_[0]->SetVariantType(VariantType::VAR_BOOL); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Input_ButtonListener::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	if (inSolveInstance[0].GetPtr() == NULL)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	
	Button* b = (Button*)inSolveInstance[0].GetVoidPtr();
	String typeName = b->GetTypeName();
	bool mute = inSolveInstance[1].GetBool();
	if (b && typeName == Button::GetTypeNameStatic())
	{
		if (!mute)
			SubscribeToEvent(b, E_PRESSED, URHO3D_HANDLER(Input_ButtonListener, HandleButtonPress));			
		else
			UnsubscribeFromEvent(E_PRESSED);			

		outSolveInstance[0] = b->IsSelected();
	}
}

void Input_ButtonListener::HandleButtonPress(StringHash eventType, VariantMap& eventData)
{
	solvedFlag_ = 0;
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}