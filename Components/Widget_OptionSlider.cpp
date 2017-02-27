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


#include "Widget_OptionSlider.h"
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>

using namespace Urho3D;

Widget_OptionSlider::Widget_OptionSlider(Urho3D::Context* context) :Widget_Base(context)
{
	URHO3D_COPY_BASE_ATTRIBUTES(Widget_Base);
}

void Widget_OptionSlider::CustomInterface()
{
	slider = (Slider*)GetChild("MainSlider", false);
	if (slider)
	{
		SubscribeToEvent(slider, E_SLIDERCHANGED, URHO3D_HANDLER(Widget_OptionSlider, HandleSliderChanged));
	}

	minInput = (LineEdit*)GetChild("SliderMin", true);
	if (minInput)
	{
		SubscribeToEvent(minInput, E_TEXTCHANGED, URHO3D_HANDLER(Widget_OptionSlider, HandleLineEdit));
		minInput->SetText(String(minRange));
	}

	maxInput = (LineEdit*)GetChild("SliderMax", true);
	if (maxInput)
	{
		SubscribeToEvent(maxInput, E_TEXTCHANGED, URHO3D_HANDLER(Widget_OptionSlider, HandleLineEdit));
		maxInput->SetText(String(maxRange));
	}

	valInput = (LineEdit*)GetChild("CurrentValue", true);
	if (valInput)
	{
		SubscribeToEvent(valInput, E_TEXTFINISHED, URHO3D_HANDLER(Widget_OptionSlider, HandleLineEdit));
		valInput->SetText(String(slider->GetValue()));
	}

	//SetParams(0.0f, 100.0f, 0.0f);
}

Widget_OptionSlider::~Widget_OptionSlider()
{
}

void Widget_OptionSlider::SetParams(float min, float max, float val)
{
	if (minInput) {
		minInput->SetText(String(min));
	}
	if (maxInput) {
		maxInput->SetText(String(max));
	}
	if (valInput) {
		valInput->SetText(String(val));
	}

	if (slider) {
		minRange = min;
		maxRange = max;

		float range = Max(maxRange - minRange, 0.0f);
		slider->SetRange(range);

		slider->SetValue(val);

	}

}

void Widget_OptionSlider::SetLabel(String label)
{
	Text* tLabel = (Text*)GetChild("SliderLabel", false);
	if (tLabel) {
		tLabel->SetText(label);
	}
}

void Widget_OptionSlider::HandleSliderChanged(StringHash eventType, VariantMap& eventData)
{
	using namespace SliderChanged;

	float val = eventData[P_VALUE].GetFloat();

	val = minRange + val; //remap to proper range

	if (valInput)
	{
		valInput->SetText(String(val));
		valInput->SetSelected(false);
		valInput->SetFocus(false);
		valInput->SetCursorPosition(0);
	}
}

void Widget_OptionSlider::HandleLineEdit(StringHash eventType, VariantMap& eventData)
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
		}
		if (l->GetName() == "SliderMax")
		{
			maxRange = Variant(VAR_FLOAT, l->GetText()).GetFloat();
			if (maxRange < minRange) {
				maxRange = minRange;
				l->SetText(String(maxRange));
			}
		}
		if (l->GetName() == "CurrentValue")
		{
			currentValue = Variant(VAR_FLOAT, l->GetText()).GetFloat();
			currentValue = Clamp(currentValue, minRange, maxRange);
			l->SetText(String(currentValue));
		}

		float range = Abs(maxRange - minRange);
		slider->SetRange(range);
		float val = currentValue - minRange;
		slider->SetValue(val);
	}
}