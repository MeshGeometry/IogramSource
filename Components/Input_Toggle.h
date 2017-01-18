#pragma once

#include "IoComponentBase.h"

class URHO3D_API Input_Toggle : public IoComponentBase {

	URHO3D_OBJECT(Input_Toggle, IoComponentBase)

public:
	Input_Toggle(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	virtual Urho3D::String GetNodeStyle();
	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);

	void HandleButtonPress(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

};