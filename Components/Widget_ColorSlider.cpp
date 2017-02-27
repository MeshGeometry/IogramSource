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


#include "Widget_ColorSlider.h"
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>

using namespace Urho3D;

Widget_ColorSlider::Widget_ColorSlider(Urho3D::Context* context) :Widget_Base(context)
{
	URHO3D_COPY_BASE_ATTRIBUTES(Widget_Base);
	currentColor = Color(0.0f, 0.0f, 0.0f, 0.0f);
}

void Widget_ColorSlider::CustomInterface()
{
	slider_R = (Slider*)GetChild("R_Slider", false);
	if (slider_R)
	{
		slider_R->SetRange(255);
		SubscribeToEvent(slider_R, E_SLIDERCHANGED, URHO3D_HANDLER(Widget_ColorSlider, HandleSliderChanged));
	}

	slider_G = (Slider*)GetChild("G_Slider", false);
	if (slider_G)
	{
		slider_G->SetRange(255);
		SubscribeToEvent(slider_G, E_SLIDERCHANGED, URHO3D_HANDLER(Widget_ColorSlider, HandleSliderChanged));
	}

	slider_B = (Slider*)GetChild("B_Slider", false);
	if (slider_B)
	{
		slider_B->SetRange(255);
		SubscribeToEvent(slider_B, E_SLIDERCHANGED, URHO3D_HANDLER(Widget_ColorSlider, HandleSliderChanged));
	}

	slider_A = (Slider*)GetChild("A_Slider", false);
	if (slider_A)
	{
		slider_A->SetRange(1);
		SubscribeToEvent(slider_A, E_SLIDERCHANGED, URHO3D_HANDLER(Widget_ColorSlider, HandleSliderChanged));
	}

	colorInput = (LineEdit*)GetChild("RGBA_val", true);
	if (colorInput)
	{
		//SubscribeToEvent(colorInput, E_TEXTFINISHED, URHO3D_HANDLER(Widget_ColorSlider, HandleLineEdit));
		colorInput->SetText(String("todo"));
	}
	background = (Button*)GetChild("ColorDisplay", true);
	if (background)
	{
		SubscribeToEvent(background, E_SLIDERCHANGED, URHO3D_HANDLER(Widget_ColorSlider, HandleSliderChanged));
		background->SetColor(currentColor);
	}

	

	//SetParams(0.0f, 100.0f, 0.0f);
}

Widget_ColorSlider::~Widget_ColorSlider()
{
}


void Widget_ColorSlider::SetCurrentColor(Color curColor)
{
	currentColor = curColor;

}

void Widget_ColorSlider::SetLabel(String label)
{
	Text* tLabel = (Text*)GetChild("SliderLabel", false);
	if (tLabel) {
		tLabel->SetText(label);
	}
}

void Widget_ColorSlider::HandleSliderChanged(StringHash eventType, VariantMap& eventData)
{
	using namespace SliderChanged;
	Slider* s = (Slider*)eventData[P_ELEMENT].GetPtr();
	if (s) {
		if (s->GetName() == "R_Slider") {
			Color curColor = GetCurrentColor();
			float val = eventData[P_VALUE].GetFloat();
			val = val/255;
			curColor.r_ = val; 
			SetCurrentColor(curColor);
		}
		if (s->GetName() == "G_Slider") {
			Color curColor = GetCurrentColor();
			float val = eventData[P_VALUE].GetFloat();
			val = val / 255;
			curColor.g_ = val;
			SetCurrentColor(curColor);
		}
		if (s->GetName() == "B_Slider") {
			Color curColor = GetCurrentColor();
			float val = eventData[P_VALUE].GetFloat();
			val = val / 255;
			curColor.b_ = val;
			SetCurrentColor(curColor);
		}
		if (s->GetName() == "A_Slider") {
			Color curColor = GetCurrentColor();
			float val = eventData[P_VALUE].GetFloat();
			curColor.a_ = val;
			SetCurrentColor(curColor);
		}
		background->SetColor(currentColor);
	}

}

//void Widget_ColorSlider::HandleLineEdit(StringHash eventType, VariantMap& eventData)
//{
//	using namespace TextFinished;
//	LineEdit* l = (LineEdit*)eventData[P_ELEMENT].GetPtr();
//	if (l && slider_X && slider_Y && slider_Z)
//	{
//		if (l->GetName() == "SliderMin")
//		{
//			minRange = Variant(VAR_FLOAT, l->GetText()).GetFloat();
//			if (minRange > maxRange) {
//				minRange = maxRange;
//				l->SetText(String(minRange));
//			}
//		}
//		if (l->GetName() == "SliderMax")
//		{
//			maxRange = Variant(VAR_FLOAT, l->GetText()).GetFloat();
//			if (maxRange < minRange) {
//				maxRange = minRange;
//				l->SetText(String(maxRange));
//			}
//		}
//
//		float range = Abs(maxRange - minRange);
//		slider_X->SetRange(range);
//		slider_Y->SetRange(range);
//		slider_Z->SetRange(range);
//
//		float revised_X = Clamp(currentValue.x_, minRange, maxRange);
//		float revised_Y = Clamp(currentValue.y_, minRange, maxRange);
//		float revised_Z = Clamp(currentValue.z_, minRange, maxRange);
//
//		slider_X->SetValue(revised_X - minRange);
//		slider_Y->SetValue(revised_Y - minRange);
//		slider_Z->SetValue(revised_Z - minRange);
//
//		SetCurrentValue(Vector3(revised_X, revised_Y, revised_Z));
//	}
//}