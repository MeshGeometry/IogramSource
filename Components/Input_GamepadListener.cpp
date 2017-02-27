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


#include "Input_GamepadListener.h"

#include "IoGraph.h"

#include "Urho3D/Physics/PhysicsWorld.h"
#include <Urho3D/Input/Input.h>
#include <Urho3D/UI/UIEvents.h>

using namespace Urho3D;

String Input_GamepadListener::iconTexture = "Textures/Icons/Input_GamepadListener.png";

Input_GamepadListener::Input_GamepadListener(Urho3D::Context* context) : IoComponentBase(context, 2, 4)
{
	SetName("GamepadListener");
	SetFullName("Gamepad Listener");
	SetDescription("Listens for gamepad input");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("INPUT");

	inputSlots_[0]->SetName("On");
	inputSlots_[0]->SetVariableName("On");
	inputSlots_[0]->SetDescription("Listen for mouse clicks");
	inputSlots_[0]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(true);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Gamepad index");
	inputSlots_[1]->SetVariableName("GI");
	inputSlots_[1]->SetDescription("Gamepad Index");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(0);
	inputSlots_[1]->DefaultSet();


	outputSlots_[0]->SetName("Button ID");
	outputSlots_[0]->SetVariableName("BI");
	outputSlots_[0]->SetDescription("Button ID");
	outputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Button Value");
	outputSlots_[1]->SetVariableName("BV");
	outputSlots_[1]->SetDescription("Button Value");
	outputSlots_[1]->SetVariantType(VariantType::VAR_BOOL);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[2]->SetName("Axis ID");
	outputSlots_[2]->SetVariableName("AI");
	outputSlots_[2]->SetDescription("Axis ID");
	outputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[2]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[3]->SetName("Axis Value");
	outputSlots_[3]->SetVariableName("AV");
	outputSlots_[3]->SetDescription("Axis Value");
	outputSlots_[3]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[3]->SetDataAccess(DataAccess::ITEM);
}


void Input_GamepadListener::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	//subscribe or unsuscribe
	bool currFlag = inSolveInstance[0].GetBool();
	gamePadID = inSolveInstance[1].GetInt();
	if (currFlag && !isOn)
	{
		SubscribeToEvent(E_JOYSTICKBUTTONDOWN, URHO3D_HANDLER(Input_GamepadListener, HandleButtonDown));
		SubscribeToEvent(E_JOYSTICKAXISMOVE, URHO3D_HANDLER(Input_GamepadListener, HandleAxisMove));
		isOn = true;
	}
	if (!currFlag && isOn)
	{
		UnsubscribeFromEvent(E_JOYSTICKBUTTONDOWN);
		UnsubscribeFromEvent(E_JOYSTICKAXISMOVE);
		isOn = false;
	}

	outSolveInstance[0] = buttonID;
	outSolveInstance[1] = buttonValue;
	outSolveInstance[2] = axisID;
	outSolveInstance[3] = axisValue;

}


void Input_GamepadListener::HandleButtonDown(StringHash eventType, VariantMap& eventData)
{
	using namespace JoystickButtonDown;

	StringHash joystickID = eventData[P_JOYSTICKID].GetStringHash();

	Input* inp = GetSubsystem<Input>();
	if (inp->GetNumJoysticks() > gamePadID)
	{
		JoystickState* state = inp->GetJoystickByIndex(gamePadID);
		if (state)
		{
			buttonID = eventData[P_BUTTON].GetInt();
			buttonValue = true;
		}
	}

	solvedFlag_ = 0;
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}

void Input_GamepadListener::HandleAxisMove(StringHash eventType, VariantMap& eventData)
{
	using namespace JoystickAxisMove;

	int axisId = eventData[P_AXIS].GetInt();

	Input* inp = GetSubsystem<Input>();
	if (inp->GetNumJoysticks() > gamePadID)
	{
		JoystickState* state = inp->GetJoystickByIndex(gamePadID);
		if (state)
		{
			axisID = eventData[P_AXIS].GetInt();
			axisValue = eventData[P_POSITION].GetFloat();
		}
	}

	solvedFlag_ = 0;
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}