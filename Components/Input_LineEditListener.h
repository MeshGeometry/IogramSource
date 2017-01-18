#pragma once

#include "IoComponentBase.h"

class URHO3D_API Input_LineEditListener : public IoComponentBase {

	URHO3D_OBJECT(Input_LineEditListener, IoComponentBase)

public:
	Input_LineEditListener(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	void HandleLineEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};