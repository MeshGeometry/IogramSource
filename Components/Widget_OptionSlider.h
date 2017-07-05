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

class URHO3D_API Widget_OptionSlider : public Widget_Base {

	URHO3D_OBJECT(Widget_OptionSlider, Widget_Base)

public:
	Widget_OptionSlider(Urho3D::Context* context);
	~Widget_OptionSlider();
	void CustomInterface();
	void SetParams(float min, float max, float val);
	void SetLabel(Urho3D::String label);
    
    float GetMin() { return minRange; }
 //   float GetRange()


private:

	float minRange = 0.0f;
	float maxRange = 100.0f;
	float currentValue = 0.0f;
	float precision = 0.001f;

	Urho3D::SharedPtr<Urho3D::Slider> slider;
	Urho3D::SharedPtr<Urho3D::LineEdit> minInput;
	Urho3D::SharedPtr<Urho3D::LineEdit> maxInput;
	Urho3D::SharedPtr<Urho3D::LineEdit> valInput;

	void HandleSliderChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleLineEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};
