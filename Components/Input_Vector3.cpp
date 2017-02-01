#include "Input_Vector3.h"

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "IoGraph.h"

using namespace Urho3D;

String Input_Vector3::iconTexture = "Textures/Icons/DefaultIcon.png";

Input_Vector3::Input_Vector3(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("Vector3");
	SetFullName("Vector3 Selector");
	SetDescription("Slider for selecting a Vector3");

	AddInputSlot(
		"Default",
		"N",
		"Vector3",
		VAR_VECTOR3,
		ITEM,
		Vector3::ZERO
		);


	AddOutputSlot(
		"Vector3",
		"V",
		"Vector output",
		VAR_PTR,
		ITEM
		);

}

String Input_Vector3::GetNodeStyle() {
	return "Component_Vector3Slider";
}

void Input_Vector3::HandleCustomInterface(UIElement* customElement) {

	mySlider = customElement->CreateChild<Widget_Vector3Slider>();
	XMLFile* styleFile = GetSubsystem<ResourceCache>()->GetResource<XMLFile>("UI/IogramDefaultStyle.xml");
	mySlider->SetStyle("Widget_Vector3Slider", styleFile);
	mySlider->CustomInterface();

	customElement->SetMinSize(150, 120);

	Vector3 savedValue = Vector3(
		GetGenericData("X").GetFloat(),
		GetGenericData("Y").GetFloat(),
		GetGenericData("Z").GetFloat());

	float savedMinValue = GetGenericData("min").GetFloat();
	float savedMaxValue = GetGenericData("max").GetFloat();
	float range = Abs(savedMaxValue - savedMinValue);

	mySlider->SetParams(savedMinValue, savedMaxValue, savedValue);
	Slider* Slider_X = mySlider->GetSlider_X();
	Slider* Slider_Y = mySlider->GetSlider_Y();
	Slider* Slider_Z = mySlider->GetSlider_Z();
	LineEdit* minEdit = mySlider->GetLineEditMin();
	LineEdit* maxEdit = mySlider->GetLineEditMax();

	if (Slider_X) {
		SubscribeToEvent(Slider_X, E_SLIDERCHANGED, URHO3D_HANDLER(Input_Vector3, HandleSliderChanged_X));
		
		Slider_X->SetRange(range);
		Slider_X->SetValue(GetGenericData("X").GetFloat());
	}

	if (Slider_Y) {
		SubscribeToEvent(Slider_Y, E_SLIDERCHANGED, URHO3D_HANDLER(Input_Vector3, HandleSliderChanged_Y));
		Slider_Y->SetRange(range);
		Slider_Y->SetValue(GetGenericData("Y").GetFloat());
	}

	if (Slider_Z) {
		SubscribeToEvent(Slider_Z, E_SLIDERCHANGED, URHO3D_HANDLER(Input_Vector3, HandleSliderChanged_Z));
		Slider_Z->SetRange(range);
		Slider_Z->SetValue(GetGenericData("Z").GetFloat());
	}
	
	if (minEdit) {
		SubscribeToEvent(minEdit, E_TEXTFINISHED, URHO3D_HANDLER(Input_Vector3, HandleLineEditMin));
		minEdit->SetText(String(savedMinValue));
	}

	if (maxEdit) {
		SubscribeToEvent(mySlider->GetLineEditMax(), E_TEXTFINISHED, URHO3D_HANDLER(Input_Vector3, HandleLineEditMax));
		maxEdit->SetText(String(savedMaxValue));
	}
}

// ensure labels are saved to disk
void Input_Vector3::HandleLineEditMin(StringHash eventType, VariantMap& eventData)
{
	using namespace TextFinished;
	LineEdit* l = (LineEdit*)eventData[P_ELEMENT].GetPtr();
	float min = Variant(VAR_FLOAT, l->GetText()).GetFloat();
	SetGenericData("min", min);
	PushChange(mySlider->GetCurrentValue());
}

void Input_Vector3::HandleLineEditMax(StringHash eventType, VariantMap& eventData)
{
	using namespace TextFinished;
	LineEdit* l = (LineEdit*)eventData[P_ELEMENT].GetPtr();
	float max = Variant(VAR_FLOAT, l->GetText()).GetFloat();
	SetGenericData("max", max);
	PushChange(mySlider->GetCurrentValue());
}

void Input_Vector3::HandleSliderChanged_X(StringHash eventType, VariantMap& eventData)
{
	using namespace SliderChanged;
	float val = eventData[P_VALUE].GetFloat();
	SetGenericData("X", val);
	PushChange(mySlider->GetCurrentValue());
}

void Input_Vector3::HandleSliderChanged_Y(StringHash eventType, VariantMap& eventData)
{
	using namespace SliderChanged;
	float val = eventData[P_VALUE].GetFloat();
	SetGenericData("Y", val);
	PushChange(mySlider->GetCurrentValue());
}

void Input_Vector3::HandleSliderChanged_Z(StringHash eventType, VariantMap& eventData)
{
	using namespace SliderChanged;
	float val = eventData[P_VALUE].GetFloat();
	SetGenericData("Z", val);
	Vector3 test = mySlider->GetCurrentValue();
	PushChange(mySlider->GetCurrentValue());
}

void Input_Vector3::PushChange(Vector3 val)
{
	Vector<int> path;
	path.Push(0);
	Variant var(val);
	IoDataTree tree(GetContext());
	tree.Add(path, var);
	InputHardSet(0, tree);
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}

void Input_Vector3::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	outSolveInstance[0] = inSolveInstance[0];

}
