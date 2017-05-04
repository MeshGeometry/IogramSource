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

#include "MultiSlider.h"
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture.h>
#include <Urho3D/Core/StringUtils.h>

using namespace Urho3D;

MultiSlider::MultiSlider(Context* context) : BorderImage(context),
min_(0.0f),
max_(1.0f),
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
	SetColor(Color::TRANSPARENT);

	//set the size
	SetMinWidth(120);

	//add the min max input region
	UIElement* rangeRegion = CreateChild<UIElement>("RangeRegion");
	rangeRegion->SetMinHeight(20);
	rangeRegion->SetLayout(LM_HORIZONTAL, 2);

	Text* minText = rangeRegion->CreateChild<Text>("MinText");
	minText->SetStyle("SmallLabelText");
	minText->SetText("Min: ");

	minEdit_ = rangeRegion->CreateChild<LineEdit>("MinEdit");
	minEdit_->SetStyleAuto();
	minEdit_->SetText(String(min_));
	minEdit_->SetMinWidth(40);
	
	Text* maxText = rangeRegion->CreateChild<Text>("MaxText");
	maxText->SetStyle("SmallLabelText");
	maxText->SetText("Max: ");

	maxEdit_ = rangeRegion->CreateChild<LineEdit>("MaxEdit");
	maxEdit_->SetStyleAuto();
	maxEdit_->SetText(String(max_));
	maxEdit_->SetMinWidth(40);

	SubscribeToEvent(minEdit_, E_TEXTFINISHED, URHO3D_HANDLER(MultiSlider, HandleTextFinished));
	SubscribeToEvent(maxEdit_, E_TEXTFINISHED, URHO3D_HANDLER(MultiSlider, HandleTextFinished));

	//add at least one slider
	AddSlider();

}

void MultiSlider::SetRange(float start, float end)
{
	min_ = start;
	max_ = Clamp(end, start + M_EPSILON, M_LARGE_VALUE);

	//loop through all sliders and update
	for (int i = 0; i < sliders_.Size(); i++) {

		float lastRange = sliders_[i]->GetRange();
		float lastValue = sliders_[i]->GetValue();

		//create new range
		float newRange = Abs(max_ - min_);
		sliders_[i]->SetRange(newRange);

		//update the value
		float newValue = (lastValue / lastRange) * newRange;
		sliders_[i]->SetValue(newValue);
	}

}

Urho3D::Slider* MultiSlider::AddSlider()
{
	//init the sliders
	Slider* newSlider = CreateChild<Slider>("RedSlider");
	newSlider->SetStyle("ThinSlider");
	newSlider->SetFixedHeight(10);

	//make sure to remap range
	float range = Abs(max_ - min_);
	newSlider->SetRange(range);

	//subscribe
	SubscribeToEvent(newSlider, E_SLIDERCHANGED, URHO3D_HANDLER(MultiSlider, HandleSliderChanged));

	//push to list
	sliders_.Push(newSlider);
	return newSlider;
}

void MultiSlider::RemoveSlider(int id)
{
	if (id < sliders_.Size() && sliders_.Size() > 1) {
		sliders_.Erase(id);
	}
}

float MultiSlider::GetSingleNumber()
{
	assert(!sliders_.Empty());

	float rawValue = sliders_[0]->GetValue();

	//remapped
	rawValue = min_ + rawValue;

	return rawValue;
}

float MultiSlider::GetNumber(int id)
{
	float value = 0.0f;
	if (id >= 0 && id < sliders_.Size()) {

		value = sliders_[id]->GetValue();
		value = min_ + value;
	}

	return value;
}

void MultiSlider::SetNumber(int id, float value)
{
	if (id >= 0 && id < sliders_.Size()) {

		muteEvents_ = true;

		sliders_[id]->SetValue(value);

		muteEvents_ = false;
	}
}

Urho3D::Vector3 MultiSlider::GetVector3()
{
	assert(sliders_.Size() >= 3);

	float x = GetNumber(0);
	float y = GetNumber(1);
	float z = GetNumber(2);

	return Vector3(x, y, z);
}

void MultiSlider::SetVector3(Vector3 value)
{
	assert(sliders_.Size() >= 3);

	muteEvents_ = true;
	SetNumber(0, value.x_);
	SetNumber(1, value.y_);
	SetNumber(2, value.z_);
	muteEvents_ = false;

}

Urho3D::Vector4 MultiSlider::GetVector4()
{
	assert(sliders_.Size() >= 4);

	float x = GetNumber(0);
	float y = GetNumber(1);
	float z = GetNumber(2);
	float w = GetNumber(3);

	return Vector4(x, y, z, w);
}

Urho3D::Color MultiSlider::GetColor()
{
	assert(sliders_.Size() >= 4);

	float x = GetNumber(0);
	float y = GetNumber(1);
	float z = GetNumber(2);
	float w = GetNumber(3);

	return Color(x, y, z, w);
}

Urho3D::Vector<float> MultiSlider::GetAllValues()
{
	Vector<float> values;

	return values;
}

void MultiSlider::HandleSliderChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	SendEvent("MultiSliderChanged");
}

void MultiSlider::HandleTextFinished(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace TextFinished;

	if (minEdit_ && maxEdit_) {
		float newMin = ToFloat(minEdit_->GetText());
		float newMax = ToFloat(maxEdit_->GetText());
		SetRange(newMin, newMax);

		minEdit_->SetCursorPosition(0);
		maxEdit_->SetCursorPosition(0);
	}
}