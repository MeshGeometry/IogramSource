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


#include "Input_Toggle.h"
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include "ColorDefs.h"

#include "IoGraph.h"

using namespace Urho3D;

String Input_Toggle::iconTexture = "Textures/Icons/Input_Toggle.png";

Input_Toggle::Input_Toggle(Urho3D::Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("Toggle");
	SetFullName("Boolean Toggle");
	SetDescription("Boolean Toggle input");
	SetGroup(IoComponentGroup::PARAMS);
	SetSubgroup("");

	inputSlots_[0]->SetName("");
	inputSlots_[0]->SetVariableName("");
	inputSlots_[0]->SetDescription("");
	inputSlots_[0]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(false);
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("");
	outputSlots_[0]->SetVariableName("");
	outputSlots_[0]->SetDescription("");
	outputSlots_[0]->SetVariantType(VariantType::VAR_BOOL); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

}

void Input_Toggle::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	outSolveInstance[0] = inSolveInstance[0];
}

String Input_Toggle::GetNodeStyle()
{
	return "BoolToggle";
}

void Input_Toggle::HandleCustomInterface(UIElement* customElement)
{
	Button* b = customElement->CreateChild<Button>("BoolToogle");
	b->SetStyle("Button");
	b->SetMinSize(30, 20);
	bool lastVal = GetGenericData("ButtonValue").GetBool();
	lastVal ? b->SetColor(LIGHT_GREEN) : b->SetColor(BRIGHT_ORANGE);
	SubscribeToEvent(b, E_PRESSED, URHO3D_HANDLER(Input_Toggle, HandleButtonPress));

}

void Input_Toggle::HandleButtonPress(StringHash eventType, VariantMap& eventData)
{
	using namespace Pressed;
	Button* b = (Button*)eventData[P_ELEMENT].GetVoidPtr();
	if (b)
	{
		if (b->IsSelected())
		{
			b->SetSelected(false);
			b->SetColor(BRIGHT_ORANGE);
			SetGenericData("ButtonValue", false);

		}
		else
		{
			b->SetSelected(true);
			b->SetColor(LIGHT_GREEN);
			SetGenericData("ButtonValue", true);
		}

		Vector<int> path;
		path.Push(0);
		Variant var(b->IsSelected());
		IoDataTree tree(GetContext());
		tree.Add(path, var);
		InputHardSet(0, tree);

		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}
}