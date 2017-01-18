#pragma once

#include "IoComponentBase.h"

class URHO3D_API Input_ScreenToggle : public IoComponentBase {

	URHO3D_OBJECT(Input_ScreenToggle, IoComponentBase)

public:
	Input_ScreenToggle(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	Urho3D::Vector<Urho3D::UIElement*> trackedItems;
	void HandleButtonPress(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleButtonRelease(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	bool toggleMode = true;
};