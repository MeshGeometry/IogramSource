#pragma once

#include "IoComponentBase.h"
#include <Urho3D/UI/UIElement.h>
#include "Widget_Vector3Slider.h"

class URHO3D_API Input_Vector3 : public IoComponentBase {
	URHO3D_OBJECT(Input_Vector3, IoComponentBase)
public:
	Input_Vector3(Urho3D::Context* context);

	virtual Urho3D::String GetNodeStyle();
	Widget_Vector3Slider* mySlider;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);


	static Urho3D::String iconTexture;
	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);

	void HandleSliderChanged_X(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleSliderChanged_Y(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleSliderChanged_Z(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleLineEditMin(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleLineEditMax(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	void PushChange(Urho3D::Vector3 val);
};