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
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Slider.h>

class URHO3D_API ColorSlider : public Urho3D::BorderImage
{
	URHO3D_OBJECT(ColorSlider, Urho3D::BorderImage)

public:

	ColorSlider(Urho3D::Context* context);
	~ColorSlider() {};

	//static void RegisterObject(Urho3D::Context* context);
	//virtual void OnResize(const Urho3D::IntVector2& newSize, const Urho3D::IntVector2& delta);

	Urho3D::Color GetCurrentColor() { return color_; }
	void SetCurrentColor(Urho3D::Color col);

protected:

	bool muteEvents_;
	
	Urho3D::Color color_;

	Urho3D::Slider* redSlider_;
	Urho3D::Slider* greenSlider_;
	Urho3D::Slider* blueSlider_;
	Urho3D::Slider* alphaSlider_;

	void SetColorFromSliders();
	void SetSlidersFromColor();

	void HandleSliderChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};