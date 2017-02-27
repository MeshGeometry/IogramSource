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


#include "Input_Slider.h"
#include <Urho3D/Core/Variant.h>
#include <Urho3D/UI/Slider.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>

#include "IoGraph.h"

using namespace Urho3D;

/*
Override some parameters
*/
String Input_Slider::iconTexture = "Textures/Icons/Input_Slider.png";
Input_Slider::Input_Slider(Urho3D::Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("Slider");
	SetFullName("Slider");
	SetDescription("An interactive numerical slider.");
	SetGroup(IoComponentGroup::PARAMS);
	SetSubgroup("");

	inputSlots_[0]->SetName("I");
	inputSlots_[0]->SetVariableName("");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant(0.0f));
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("Value");
	outputSlots_[0]->SetVariableName("");
	outputSlots_[0]->SetDescription("");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}
void Input_Slider::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	outSolveInstance[0] = inSolveInstance[0];
};
String Input_Slider::GetNodeStyle()
{
	return "SliderNode";
}
void Input_Slider::HandleCustomInterface(UIElement* customElement)
{

	//get the meta data
	minRange = GetGenericData("min").GetFloat();
	maxRange = GetGenericData("max").GetFloat();
	currentValue = GetGenericData("value").GetFloat();

	customElement->SetMinSize(150, 60);

	slider = customElement->CreateChild<Slider>("MainSlider", true);
	slider->SetStyle("ThinSlider");
	//slider->SetMinHeight(20);
	slider->SetSize(customElement->GetSize().x_, 12);
	//slider->SetMaxHeight(30);

	UIElement* controls = customElement->CreateChild<UIElement>("ControlGroup");
	controls->SetMinHeight(30);
	controls->SetLayoutMode(LM_HORIZONTAL);
	controls->SetLayoutSpacing(2);

	minInput = controls->CreateChild<LineEdit>("SliderMin");
	minInput->SetStyleAuto();
	minInput->SetText(String(minRange));

	maxInput = controls->CreateChild<LineEdit>("SliderMax");
	maxInput->SetStyleAuto();
	maxInput->SetText(String(maxRange));

	valInput = controls->CreateChild<LineEdit>("CurrentValue");
	valInput->SetStyleAuto();
	valInput->SetText(String(currentValue));

	SubscribeToEvent(slider, E_SLIDERCHANGED, URHO3D_HANDLER(Input_Slider, HandleSliderChanged));
	SubscribeToEvent(valInput, E_TEXTFINISHED, URHO3D_HANDLER(Input_Slider, HandleLineEdit));
	SubscribeToEvent(maxInput, E_TEXTCHANGED, URHO3D_HANDLER(Input_Slider, HandleLineEdit));
	SubscribeToEvent(minInput, E_TEXTCHANGED, URHO3D_HANDLER(Input_Slider, HandleLineEdit));


	//set the value
	float range = Abs(maxRange - minRange);
	slider->SetRange(range);
	float val = currentValue - minRange;
	slider->SetValue(val);

}
void Input_Slider::HandleSliderChanged(StringHash eventType, VariantMap& eventData)
{
	using namespace SliderChanged;

	Slider* s = (Slider*)eventData[P_ELEMENT].GetPtr();
	if (s != slider)
		return;

	float val = eventData[P_VALUE].GetFloat();
	val = minRange + val; //remap to proper range
						  //valText->SetText(String(val));
	Vector<int> path;
	path.Push(0);
	Variant var(val);
	IoDataTree tree(GetContext());
	tree.Add(path, var);
	InputHardSet(0, tree);
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();

	if (valInput)
	{
		valInput->SetText(String(val));
		valInput->SetSelected(false);
		valInput->SetFocus(false);
		valInput->SetCursorPosition(0);

		SetGenericData("value", val);
	}
}
void Input_Slider::HandleLineEdit(StringHash eventType, VariantMap& eventData)
{
	using namespace TextChanged;
	LineEdit* l = (LineEdit*)eventData[P_ELEMENT].GetPtr();
	if (l && slider)
	{
		if (l->GetName() == "SliderMin")
		{
			minRange = Variant(VAR_FLOAT, l->GetText()).GetFloat();
			if (minRange > maxRange) {
				minRange = maxRange;
				l->SetText(String(minRange));
			}
			SetGenericData("min", minRange);
		}
		if (l->GetName() == "SliderMax")
		{
			maxRange = Variant(VAR_FLOAT, l->GetText()).GetFloat();
			if (maxRange < minRange) {
				maxRange = minRange;
				l->SetText(String(maxRange));
			}
			SetGenericData("max", maxRange);
		}
		if (l->GetName() == "CurrentValue")
		{
			currentValue = Variant(VAR_FLOAT, l->GetText()).GetFloat();
			currentValue = Clamp(currentValue, minRange, maxRange);
			l->SetText(String(currentValue));
			SetGenericData("value", currentValue);
		}

		float range = Abs(maxRange - minRange);
		slider->SetRange(range);
		float val = currentValue - minRange;
		slider->SetValue(val);
	}
}