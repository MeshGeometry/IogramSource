#pragma once

#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/LineEdit.h>
#include "IoComponentBase.h"

class URHO3D_API Input_Float : public IoComponentBase {

	URHO3D_OBJECT(Input_Float, IoComponentBase)

public:
	Input_Float(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	virtual Urho3D::String GetNodeStyle();
	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);
	static Urho3D::String iconTexture;

	Urho3D::LineEdit* floatNameEdit;
	Urho3D::String floatName;
	void HandleLineEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};
