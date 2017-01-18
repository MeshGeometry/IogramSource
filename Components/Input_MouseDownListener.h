#pragma once

#include "IoComponentBase.h"

class URHO3D_API Input_MouseDownListener : public IoComponentBase {

	URHO3D_OBJECT(Input_MouseDownListener, IoComponentBase)

public:
	Input_MouseDownListener(Urho3D::Context* context);

	static Urho3D::String iconTexture;


	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	void HandleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	bool isOn = false;
	Urho3D::Vector3 mPos;
	Urho3D::Vector3 mDelta;
	int mButton = 1;

};