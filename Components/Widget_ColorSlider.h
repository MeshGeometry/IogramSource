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