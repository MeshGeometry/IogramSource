#pragma once

#include "IoComponentBase.h"

class URHO3D_API Input_GamepadListener : public IoComponentBase {

	URHO3D_OBJECT(Input_GamepadListener, IoComponentBase)

public:
	Input_GamepadListener(Urho3D::Context* context);

	static Urho3D::String iconTexture;


	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);
	bool isOn = false;

	int buttonID = -1;
	bool buttonValue = false;
	int axisID = -1;
	float axisValue;

	int gamePadID = 0;
	void HandleButtonDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleAxisMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};