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


#pragma once
#include "Widget_Base.h"

#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Slider.h>

class URHO3D_API Widget_Vector3Slider : public Widget_Base {

	URHO3D_OBJECT(Widget_Vector3Slider, Widget_Base)

public:
	Widget_Vector3Slider(Urho3D::Context* context);
	~Widget_Vector3Slider();
	void CustomInterface();
	void SetParams(float min, float max, Urho3D::Vector3 val);
	void SetLabel(Urho3D::String label);

	Urho3D::Slider* GetSlider_X() { return slider_X; };
	Urho3D::Slider* GetSlider_Y() { return slider_Y; };
	Urho3D::Slider* GetSlider_Z() { return slider_Z; };

	Urho3D::LineEdit* GetLineEditMin() { return minInput; };
	Urho3D::LineEdit* GetLineEditMax() { return maxInput; };

	Urho3D::Vector3 GetCurrentValue() { return currentValue; };
	void SetCurrentValue(Urho3D::Vector3 value);

private:

	float minRange = 0.0f;
	float maxRange = 100.0f;
	Urho3D::Vector3 currentValue = Urho3D::Vector3::ZERO;
	float precision = 0.001f;

	Urho3D::SharedPtr<Urho3D::Slider> slider_X;
	Urho3D::SharedPtr<Urho3D::Slider> slider_Y;
	Urho3D::SharedPtr<Urho3D::Slider> slider_Z;
	Urho3D::SharedPtr<Urho3D::LineEdit> minInput;
	Urho3D::SharedPtr<Urho3D::LineEdit> maxInput;

	void HandleLineEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleSliderChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

};