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


#include "Input_ScreenToggle.h"
#include "Urho3D/UI/Button.h"
#include "Urho3D/UI/Text.h"
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/UI.h>
#include "ColorDefs.h"
#include "Widget_Base.h"
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Input_ScreenToggle::iconTexture = "Textures/Icons/Input_ScreenToggle.png";

Input_ScreenToggle::Input_ScreenToggle(Urho3D::Context* context) : IoComponentBase(context, 5, 1)
{
	SetName("ScreenToggle");
	SetFullName("Screen Toggle");
	SetDescription("Adds a boolean toggle to the user interface");
	SetGroup(IoComponentGroup::USER);
	SetSubgroup("INPUT");

	inputSlots_[0]->SetName("P");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Position of the button in screen coordinates.");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant());
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Label");
	inputSlots_[1]->SetVariableName("L");
	inputSlots_[1]->SetDescription("Label of the button");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue("Option: ");
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Toggle");
	inputSlots_[2]->SetVariableName("T");
	inputSlots_[2]->SetDescription("Toggle mode");
	inputSlots_[2]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(false);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("Priority");
	inputSlots_[3]->SetVariableName("I");
	inputSlots_[3]->SetDescription("Priority Index");
	inputSlots_[3]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(0);
	inputSlots_[3]->DefaultSet();

	inputSlots_[4]->SetName("Parent");
	inputSlots_[4]->SetVariableName("PE");
	inputSlots_[4]->SetDescription("Optional parent");
	inputSlots_[4]->SetVariantType(VariantType::VAR_PTR);
	inputSlots_[4]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("Button Pointer");
	outputSlots_[0]->SetVariableName("Ptr");
	outputSlots_[0]->SetDescription("Pointer to the UI Element");
	outputSlots_[0]->SetVariantType(VariantType::VAR_PTR); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

}

void Input_ScreenToggle::PreLocalSolve()
{
	//get the ui subsystem
	UI* ui = GetSubsystem<UI>();

	//delete all existing ui elements
	for (int i = 0; i < trackedItems.Size(); i++)
	{
		trackedItems[i]->Remove();
	}

	trackedItems.Clear();
}

void Input_ScreenToggle::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	//get the ui subsystem
	UI* ui = GetSubsystem<UI>();

	bool useCoords = false;
	if (inSolveInstance[0].GetType() == VAR_VECTOR3)
	{
		useCoords = true;
	}

	String label = inSolveInstance[1].GetString();
	toggleMode = inSolveInstance[2].GetBool();
	int idx = inSolveInstance[3].GetInt();
	UIElement* parent = (UIElement*)inSolveInstance[4].GetPtr();

	//get the active ui region
	UIElement* root = (UIElement*)GetGlobalVar("activeUIRegion").GetPtr();
	if (!root)
	{
		SetAllOutputsNull(outSolveInstance);
	}

	//attach
	UIElement* container = (parent == NULL) ? root : parent;

	Widget_Base* b = (Widget_Base*)container->CreateChild<Button>("CustomUI", idx);
	XMLFile* styleFile = GetSubsystem<ResourceCache>()->GetResource<XMLFile>("UI/IogramDefaultStyle.xml");
	b->SetStyle("ScreenButton", styleFile);

	if (useCoords)
	{
		Vector3 coords = inSolveInstance[0].GetVector3();
		b->SetPosition(coords.x_, coords.y_);
	}

	Button* actingButton = (Button*)b->GetChild("ButtonOption", false);
	if (actingButton) {
		SubscribeToEvent(actingButton, E_PRESSED, URHO3D_HANDLER(Input_ScreenToggle, HandleButtonPress));
		SubscribeToEvent(actingButton, E_RELEASED, URHO3D_HANDLER(Input_ScreenToggle, HandleButtonRelease));
		actingButton->SetColor(BRIGHT_ORANGE);
	}

	Text* tLabel = (Text*)b->GetChild("ButtonLabel", false);
	if (tLabel) {
		tLabel->SetText(label);
	}
	

	trackedItems.Push(b);

	outSolveInstance[0] = actingButton;
}

void Input_ScreenToggle::HandleButtonPress(StringHash eventType, VariantMap& eventData)
{
	using namespace Pressed;
	Button* cb = (Button*)eventData[P_ELEMENT].GetVoidPtr();
	bool sel = cb->IsSelected();
	if (toggleMode)
	{
		if (!sel)
		{
			cb->SetSelected(true);
			cb->SetColor(LIGHT_GREEN);
			//((Text*)cb->GetVar("TF_TXT").GetPtr())->SetText("True");
		}
		else
		{
			cb->SetSelected(false);
			cb->SetColor(BRIGHT_ORANGE);
			//((Text*)cb->GetVar("TF_TXT").GetPtr())->SetText("False");
		}
	}
	else
	{
		cb->SetColor(BRIGHT_ORANGE);
	}
}

void Input_ScreenToggle::HandleButtonRelease(StringHash eventType, VariantMap& eventData)
{
	using namespace Released;
	Button* cb = (Button*)eventData[P_ELEMENT].GetVoidPtr();
	if (!toggleMode)
	{
		cb->SetColor(LIGHT_GREEN);
	}
}