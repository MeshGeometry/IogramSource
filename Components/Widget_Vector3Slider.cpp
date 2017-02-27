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


#include "Widget_Vector3Slider.h"
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>

using namespace Urho3D;

Widget_Vector3Slider::Widget_Vector3Slider(Urho3D::Context* context) :Widget_Base(context)
{
	URHO3D_COPY_BASE_ATTRIBUTES(Widget_Base);
}

void Widget_Vector3Slider::CustomInterface()
{
	slider_X = (Slider*)GetChild("X_Slider", false);
	if (slider_X)
	{
		SubscribeToEvent(slider_X, E_SLIDERCHANGED, URHO3D_HANDLER(Widget_Vector3Slider, HandleSliderChanged));
	}

	slider_Y = (Slider*)GetChild("Y_Slider", false);
	if (slider_Y)
	{
		SubscribeToEvent(slider_Y, E_SLIDERCHANGED, URHO3D_HANDLER(Widget_Vector3Slider, HandleSliderChanged));
	}

	slider_Z = (Slider*)GetChild("Z_Slider", false);
	if (slider_Z)
	{
		SubscribeToEvent(slider_Z, E_SLIDERCHANGED, URHO3D_HANDLER(Widget_Vector3Slider, HandleSliderChanged));
	}

	minInput = (LineEdit*)GetChild("SliderMin", true);
	if (minInput)
	{
		SubscribeToEvent(minInput, E_TEXTFINISHED, URHO3D_HANDLER(Widget_Vector3Slider, HandleLineEdit));
		minInput->SetText(String(minRange));
	}

	maxInput = (LineEdit*)GetChild("SliderMax", true);
	if (maxInput)
	{
		SubscribeToEvent(maxInput, E_TEXTFINISHED, URHO3D_HANDLER(Widget_Vector3Slider, HandleLineEdit));
		maxInput->SetText(String(maxRange));
	}

	//SetParams(0.0f, 100.0f, 0.0f);
}

Widget_Vector3Slider::~Widget_Vector3Slider()
{
}

void Widget_Vector3Slider::SetParams(float min, float max, Vector3 val)
{
	if (minInput) {
		minInput->SetText(String(min));
	}
	if (maxInput) {
		maxInput->SetText(String(max));
	}

	if (slider_X && slider_Y && slider_Z) {
		minRange = min;
		maxRange = max;

		float range = Max(maxRange - minRange, 0.0f);
		slider_X->SetRange(range);
		slider_Y->SetRange(range);
		slider_Z->SetRange(range);

		slider_X->SetValue(val.x_);
		slider_Y->SetValue(val.y_);
		slider_Z->SetValue(val.z_);

	}

}



void Widget_Vector3Slider::SetLabel(String label)
{
	Text* tLabel = (Text*)GetChild("SliderLabel", false);
	if (tLabel) {
		tLabel->SetText(label);
	}
}

void Widget_Vector3Slider::SetCurrentValue(Vector3 value)
{
	currentValue = value;
}

void Widget_Vector3Slider::HandleSliderChanged(StringHash eventType, VariantMap& eventData)
{
	using namespace SliderChanged;
	Slider* s = (Slider*)eventData[P_ELEMENT].GetPtr();
	if (s) {
		if (s->GetName() == "X_Slider") {
			Vector3 curValue = GetCurrentValue();
			float val = eventData[P_VALUE].GetFloat();
			val = minRange + val;
			curValue.x_ = val;
			SetCurrentValue(curValue);
		}
		if (s->GetName() == "Y_Slider") {
			Vector3 curValue = GetCurrentValue();
			float val = eventData[P_VALUE].GetFloat();
			val = minRange + val;
			curValue.y_ = val;
			SetCurrentValue(curValue);
		}
		if (s->GetName() == "Z_Slider") {
			Vector3 curValue = GetCurrentValue();
			float val = eventData[P_VALUE].GetFloat();
			val = minRange + val;
			curValue.z_ = val;
			SetCurrentValue(curValue);
		}
	}

}

void Widget_Vector3Slider::HandleLineEdit(StringHash eventType, VariantMap& eventData)
{
	using namespace TextFinished;
	LineEdit* l = (LineEdit*)eventData[P_ELEMENT].GetPtr();
	if (l && slider_X && slider_Y && slider_Z)
	{
		if (l->GetName() == "SliderMin")
		{
			minRange = Variant(VAR_FLOAT, l->GetText()).GetFloat();
			if (minRange > maxRange) {
				minRange = maxRange;
				l->SetText(String(minRange));
			}
		}
		if (l->GetName() == "SliderMax")
		{
			maxRange = Variant(VAR_FLOAT, l->GetText()).GetFloat();
			if (maxRange < minRange) {
				maxRange = minRange;
				l->SetText(String(maxRange));
			}
		}

		float range = Abs(maxRange - minRange);
		slider_X->SetRange(range);
		slider_Y->SetRange(range);
		slider_Z->SetRange(range);

		float revised_X = Clamp(currentValue.x_, minRange, maxRange);
		float revised_Y = Clamp(currentValue.y_, minRange, maxRange);
		float revised_Z = Clamp(currentValue.z_, minRange, maxRange);

		slider_X->SetValue(revised_X - minRange);
		slider_Y->SetValue(revised_Y - minRange);
		slider_Z->SetValue(revised_Z - minRange);

		SetCurrentValue(Vector3(revised_X, revised_Y, revised_Z));
	}
}