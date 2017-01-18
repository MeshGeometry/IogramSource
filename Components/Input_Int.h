#pragma once

#include "IoComponentBase.h"
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/LineEdit.h>

class URHO3D_API Input_Int : public IoComponentBase {

	URHO3D_OBJECT(Input_Int, IoComponentBase)

public:
	Input_Int(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	virtual Urho3D::String GetNodeStyle();
	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);
	static Urho3D::String iconTexture;

	Urho3D::LineEdit* intNameEdit;
	Urho3D::String intName;
	void HandleLineEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};