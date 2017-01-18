#pragma once

#include "IoComponentBase.h"

class URHO3D_API Input_ButtonListener : public IoComponentBase {

	URHO3D_OBJECT(Input_ButtonListener, IoComponentBase)

public:
	Input_ButtonListener(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	void HandleButtonPress(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};