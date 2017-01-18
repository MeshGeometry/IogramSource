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