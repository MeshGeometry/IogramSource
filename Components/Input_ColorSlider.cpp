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


#include "Input_ColorSlider.h"

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "IoGraph.h"

using namespace Urho3D;

String Input_ColorSlider::iconTexture = "Textures/Icons/Input_ColorWheel.png";

Input_ColorSlider::Input_ColorSlider(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("Color");
	SetFullName("Color Selector");
	SetDescription("Slider for selecting a Color");

	AddInputSlot(
		"Color",
		"C",
		"Color",
		VAR_COLOR,
		ITEM
		);


	AddOutputSlot(
		"Color_out",
		"C",
		"Color output",
		VAR_PTR,
		ITEM
		);

}

String Input_ColorSlider::GetNodeStyle() {
	return "Component_ColorSlider";
}

void Input_ColorSlider::HandleCustomInterface(UIElement* customElement) {

	mySlider = customElement->CreateChild<Widget_ColorSlider>();
	XMLFile* styleFile = GetSubsystem<ResourceCache>()->GetResource<XMLFile>("UI/IogramDefaultStyle.xml");
	mySlider->SetStyle("Widget_ColorSlider", styleFile);
	mySlider->CustomInterface();

	customElement->SetMinSize(280, 120);

	Color savedValue = Color(GetGenericData("saved_color").GetColor());


	mySlider->SetCurrentColor(savedValue);

	Slider* Slider_R = mySlider->GetSlider_R();
	Slider* Slider_G = mySlider->GetSlider_G();
	Slider* Slider_B = mySlider->GetSlider_B();
	Slider* Slider_A = mySlider->GetSlider_A();

	//LineEdit* minEdit = mySlider->GetLineEditMin();
	//LineEdit* maxEdit = mySlider->GetLineEditMax();

	if (Slider_R) {
		SubscribeToEvent(Slider_R, E_SLIDERCHANGED, URHO3D_HANDLER(Input_ColorSlider, HandleSliderChanged));
		Slider_R->SetValue(savedValue.r_);
	}
	if (Slider_G) {
		SubscribeToEvent(Slider_G, E_SLIDERCHANGED, URHO3D_HANDLER(Input_ColorSlider, HandleSliderChanged));
		Slider_G->SetValue(savedValue.g_);
	}
	if (Slider_B) {
		SubscribeToEvent(Slider_B, E_SLIDERCHANGED, URHO3D_HANDLER(Input_ColorSlider, HandleSliderChanged));
		Slider_B->SetValue(savedValue.b_);
	}
	if (Slider_A) {
		SubscribeToEvent(Slider_A, E_SLIDERCHANGED, URHO3D_HANDLER(Input_ColorSlider, HandleSliderChanged));
		Slider_A->SetValue(savedValue.a_);
	}

	//if (minEdit) {
	//	SubscribeToEvent(minEdit, E_TEXTFINISHED, URHO3D_HANDLER(Input_ColorSlider, HandleLineEditMin));
	//	minEdit->SetText(String(savedMinValue));
	//}

}

void Input_ColorSlider::HandleSliderChanged(StringHash eventType, VariantMap& eventData)
{
	using namespace SliderChanged;
	Slider* s = (Slider*)eventData[P_ELEMENT].GetPtr();

	if (s) {
		if (s->GetName() == "R_Slider") {
			float val = eventData[P_VALUE].GetFloat();
			Color newValue = Color(GetGenericData("saved_color").GetColor());
			newValue.r_ = val;
			SetGenericData("saved_color", newValue);
			PushChange(mySlider->GetCurrentColor());
		}
		if (s->GetName() == "G_Slider") {
			float val = eventData[P_VALUE].GetFloat();
			Color newValue = Color(GetGenericData("saved_color").GetColor());
			newValue.g_ = val;
			SetGenericData("saved_color", newValue);
			PushChange(mySlider->GetCurrentColor());
		}
		if (s->GetName() == "B_Slider") {
			float val = eventData[P_VALUE].GetFloat();
			Color newValue = Color(GetGenericData("saved_color").GetColor());
			newValue.b_ = val;
			SetGenericData("saved_color", newValue);
			PushChange(mySlider->GetCurrentColor());
		}
		if (s->GetName() == "A_Slider") {
			float val = eventData[P_VALUE].GetFloat();
			Color newValue = Color(GetGenericData("saved_color").GetColor());
			newValue.a_ = val;
			SetGenericData("saved_color", newValue);
			PushChange(mySlider->GetCurrentColor());
		}
	}
}

void Input_ColorSlider::PushChange(Color curColor)
{
	Vector<int> path;
	path.Push(0);
	Variant var(curColor);
	IoDataTree tree(GetContext());
	tree.Add(path, var);
	InputHardSet(0, tree);
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}

void Input_ColorSlider::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	outSolveInstance[0] = inSolveInstance[0];

}
