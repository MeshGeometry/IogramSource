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