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

class URHO3D_API Widget_ColorSlider : public Widget_Base {

	URHO3D_OBJECT(Widget_ColorSlider, Widget_Base)

public:
	Widget_ColorSlider(Urho3D::Context* context);
	~Widget_ColorSlider();
	void CustomInterface();

	void SetLabel(Urho3D::String label);

	Urho3D::Slider* GetSlider_R() { return slider_R; };
	Urho3D::Slider* GetSlider_G() { return slider_G; };
	Urho3D::Slider* GetSlider_B() { return slider_B; };
	Urho3D::Slider* GetSlider_A() { return slider_A; };

	Urho3D::LineEdit* GetLineEditColor() { return colorInput; };

	Urho3D::Color GetCurrentColor() { return currentColor; };
	void SetCurrentColor(Urho3D::Color curColor);

private:

	Urho3D::Color currentColor = Urho3D::Color(0.0f, 0.0f, 0.0f, 0.0f);

	Urho3D::Slider* slider_R;
	Urho3D::Slider* slider_G;
	Urho3D::Slider* slider_B;
	Urho3D::Slider* slider_A;
	Urho3D::LineEdit* colorInput;
	Urho3D::Button* background;

	//void HandleLineEdit(StringHash eventType, VariantMap& eventData);
	void HandleSliderChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

};