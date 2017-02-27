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

#include "IoComponentBase.h"
#include <Urho3D/UI/Slider.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/UIEvents.h>

class URHO3D_API Input_Slider : public IoComponentBase {

	URHO3D_OBJECT(Input_Slider, IoComponentBase)

public:
	Input_Slider(Urho3D::Context* context);
public:

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	float minRange = 0.0f;
	float maxRange = 100.0f;
	float currentValue = 0.000f;

	static Urho3D::String iconTexture;
	virtual Urho3D::String GetNodeStyle();
	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);
	void HandleSliderChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleLineEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	//void SetParams(float max, float min, float current);

	Urho3D::Slider* slider = NULL;
	Urho3D::LineEdit* minInput = NULL;
	Urho3D::LineEdit* maxInput = NULL;
	Urho3D::LineEdit* valInput = NULL;
};