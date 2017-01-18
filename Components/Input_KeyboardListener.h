#pragma once

#include "IoComponentBase.h"

class URHO3D_API Input_KeyboardListener : public IoComponentBase {

	URHO3D_OBJECT(Input_KeyboardListener, IoComponentBase)

public:
	Input_KeyboardListener(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	Urho3D::String keyFilter = "";
	Urho3D::String keyDown = "";
	Urho3D::String keyUp = "";
	Urho3D::String keyPress = "";

	void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleKeyUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

};