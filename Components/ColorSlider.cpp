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

#include "ColorSlider.h"
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture.h>

using namespace Urho3D;

ColorSlider::ColorSlider(Context* context) : BorderImage(context),
	color_(1.0f, 1.0f, 1.0f, 1.0f),
	muteEvents_(false)
{
	 
	XMLFile* styleFile = GetSubsystem<ResourceCache>()->GetResource<XMLFile>("UI/FlatUIStyle.xml");
	if (styleFile) {
		SetDefaultStyle(styleFile);
	}

	
	//set layout
	SetLayoutMode(LayoutMode::LM_VERTICAL);
	SetLayoutBorder(IntRect(10, 10, 10, 10));
	SetLayoutSpacing(5);
	SetBlendMode(BlendMode::BLEND_ALPHA);
	SetStyleAuto();

	//set the size
	SetSize(120, 60);

	//init the sliders
	redSlider_ = CreateChild<Slider>("RedSlider");
	redSlider_->SetStyle("ThinSlider");
	redSlider_->SetFixedHeight(10);
	redSlider_->SetRange(1.0f);
	greenSlider_ = CreateChild<Slider>("GreenSlider");
	greenSlider_->SetStyle("ThinSlider");
	greenSlider_->SetFixedHeight(10);
	greenSlider_->SetRange(1.0f);
	blueSlider_ = CreateChild<Slider>("BlueSlider");
	blueSlider_->SetStyle("ThinSlider");
	blueSlider_->SetFixedHeight(10);
	blueSlider_->SetRange(1.0f);
	alphaSlider_ = CreateChild<Slider>("AlphaSlider");
	alphaSlider_->SetStyle("ThinSlider");
	alphaSlider_->SetFixedHeight(10);
	alphaSlider_->SetRange(1.0f);

	//set the background color
	SetCurrentColor(Color::YELLOW);
	SetColorFromSliders();

	//sign up to slider events
	SubscribeToEvent(redSlider_, E_SLIDERCHANGED, URHO3D_HANDLER(ColorSlider, HandleSliderChanged));
	SubscribeToEvent(greenSlider_, E_SLIDERCHANGED, URHO3D_HANDLER(ColorSlider, HandleSliderChanged));
	SubscribeToEvent(blueSlider_, E_SLIDERCHANGED, URHO3D_HANDLER(ColorSlider, HandleSliderChanged));
	SubscribeToEvent(alphaSlider_, E_SLIDERCHANGED, URHO3D_HANDLER(ColorSlider, HandleSliderChanged));
}

void ColorSlider::SetCurrentColor(Color col)
{
	color_ = col;
	SetSlidersFromColor();
	//set the background
	SetColor(color_);
}

void ColorSlider::SetColorFromSliders()
{
	float r = redSlider_->GetValue();
	float g = greenSlider_->GetValue();
	float b = blueSlider_->GetValue();
	float a = alphaSlider_->GetValue();

	color_ = Color(r, g, b, a);

	//set the background
	SetColor(color_);

	//send event
	SendEvent("ColorSliderChanged");
}

void ColorSlider::SetSlidersFromColor()
{
	muteEvents_ = true;

	redSlider_->SetValue(color_.r_);
	greenSlider_->SetValue(color_.g_);
	blueSlider_->SetValue(color_.b_);
	alphaSlider_->SetValue(color_.a_);

	SetColor(color_);

	muteEvents_ = false;
}


void ColorSlider::HandleSliderChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	if (!muteEvents_) {
		SetColorFromSliders();
	}

}