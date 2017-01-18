#pragma once

#include "IoComponentBase.h"
#include <Urho3D/UI/UIElement.h>
#include "Widget_ColorSlider.h"

class URHO3D_API Input_ColorSlider : public IoComponentBase {
	URHO3D_OBJECT(Input_ColorSlider, IoComponentBase)
public:
	Input_ColorSlider(Urho3D::Context* context);

	virtual Urho3D::String GetNodeStyle();
	Widget_ColorSlider* mySlider;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);


	static Urho3D::String iconTexture;
	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);

	void HandleSliderChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	//void HandleSliderChanged_X(StringHash eventType, VariantMap& eventData);
	//void HandleSliderChanged_Y(StringHash eventType, VariantMap& eventData);
	//void HandleSliderChanged_Z(StringHash eventType, VariantMap& eventData);
	//void HandleLineEditMin(StringHash eventType, VariantMap& eventData);
	//void HandleLineEditMax(StringHash eventType, VariantMap& eventData);

	void PushChange(Urho3D::Color curColor);
};