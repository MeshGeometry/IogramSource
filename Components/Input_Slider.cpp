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
	slider = (Slider*)customElement->GetChild("MainSlider", false);

	//get the meta data
	minRange = GetGenericData("min").GetFloat();
	maxRange = GetGenericData("max").GetFloat();
	currentValue = GetGenericData("value").GetFloat();


	if (slider)
	{
		SubscribeToEvent(slider, E_SLIDERCHANGED, URHO3D_HANDLER(Input_Slider, HandleSliderChanged));
		Variant val;
		inputSlots_[0]->GetIoDataTreePtr()->Begin();
		inputSlots_[0]->GetIoDataTreePtr()->GetNextItem(val, DataAccess::ITEM);
		slider->SetValue(val.GetFloat());
	}
	minInput = (LineEdit*)customElement->GetChild("SliderMin", true);
	if (minInput)
	{
		SubscribeToEvent(minInput, E_TEXTCHANGED, URHO3D_HANDLER(Input_Slider, HandleLineEdit));
		minInput->SetText(String(minRange));
	}
	maxInput = (LineEdit*)customElement->GetChild("SliderMax", true);
	if (maxInput)
	{
		SubscribeToEvent(maxInput, E_TEXTCHANGED, URHO3D_HANDLER(Input_Slider, HandleLineEdit));
		maxInput->SetText(String(maxRange));
	}
	valInput = (LineEdit*)customElement->GetChild("CurrentValue", true);
	if (valInput)
	{
		SubscribeToEvent(valInput, E_TEXTFINISHED, URHO3D_HANDLER(Input_Slider, HandleLineEdit));
		valInput->SetText(String(currentValue));
	}
}
void Input_Slider::HandleSliderChanged(StringHash eventType, VariantMap& eventData)
{
	using namespace SliderChanged;
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