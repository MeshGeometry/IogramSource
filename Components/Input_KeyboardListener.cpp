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


#include "Input_KeyboardListener.h"

#include "IoGraph.h"

#include "Urho3D/Physics/PhysicsWorld.h"
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Input/Input.h>

using namespace Urho3D;

String Input_KeyboardListener::iconTexture = "Textures/Icons/Input_KeyboardListener.png";

Input_KeyboardListener::Input_KeyboardListener(Urho3D::Context* context) : IoComponentBase(context, 2, 3)
{
	SetName("KeyboardListener");
	SetFullName("Keyboard Listener");
	SetDescription("Listens for Key strokes");

	inputSlots_[0]->SetName("On");
	inputSlots_[0]->SetVariableName("On");
	inputSlots_[0]->SetDescription("Listen for mouse clicks");
	inputSlots_[0]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(true);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Key Filter");
	inputSlots_[1]->SetVariableName("K");
	inputSlots_[1]->SetDescription("Keys to listen to. If blank, return all.");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(1);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Key Press");
	outputSlots_[0]->SetVariableName("P");
	outputSlots_[0]->SetDescription("Returns once on the frame that the key has pressed.");
	outputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Key Down");
	outputSlots_[1]->SetVariableName("D");
	outputSlots_[1]->SetDescription("Returns on every frame that the key is held down.");
	outputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[2]->SetName("Key Up");
	outputSlots_[2]->SetVariableName("U");
	outputSlots_[2]->SetDescription("Returns once on the frame that the key is released.");
	outputSlots_[2]->SetVariantType(VariantType::VAR_STRING);
	outputSlots_[2]->SetDataAccess(DataAccess::ITEM);


}


void Input_KeyboardListener::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	//subscribe or unsuscribe
	if (inSolveInstance[0].GetType() == VAR_BOOL || inSolveInstance[0].GetType() == VAR_INT || inSolveInstance[0].GetType() == VAR_FLOAT)
	{
		bool val = inSolveInstance[0].GetBool();
		keyFilter = inSolveInstance[1].GetString();
		if(val)
		{
			SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Input_KeyboardListener, HandleKeyDown));
			SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(Input_KeyboardListener, HandleKeyUp));
			//SubscribeToEvent(E_KEYPRESS, URHO3D_HANDLER(Input_KeyboardListener, HandleKeyDown));
		}
		else
		{
			UnsubscribeFromAllEvents();
		}
	}
	else
	{
		outSolveInstance[0] = Variant();
		outSolveInstance[1] = Variant();
		outSolveInstance[2] = Variant();
		return;
	}

	outSolveInstance[0] = keyPress;
	outSolveInstance[1] = keyDown;
	outSolveInstance[2] = keyUp;
}


void Input_KeyboardListener::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
	using namespace KeyDown;

	int keyId = eventData[P_KEY].GetInt();
	Input* inp = GetSubsystem<Input>();
	String keyName = inp->GetKeyName(keyId);
	if (keyFilter.Empty())
	{
		keyPress = keyName;
		keyDown = keyName;
	}
	else if (keyFilter.Contains(keyName))
	{
		keyPress = keyName;
		keyDown = keyName;
	}
	else
	{
		keyPress = "";
		keyDown = "";
	}

	solvedFlag_ = 0;
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}

void Input_KeyboardListener::HandleKeyUp(StringHash eventType, VariantMap& eventData)
{
	using namespace KeyUp;

	int keyId = eventData[P_KEY].GetInt();
	Input* inp = GetSubsystem<Input>();
	String keyName = inp->GetKeyName(keyId);
	if (keyFilter.Empty())
	{
		keyUp = keyName;
	}
	else if (keyFilter.Contains(keyName))
	{
		keyUp = keyName;
	}
	else
	{
		keyUp = "";
	}

	if (keyUp == keyDown)
	{
		keyDown = "";
	}

	solvedFlag_ = 0;
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}
