#pragma once

#include "IoComponentBase.h"
#include <Urho3D/UI/UIElement.h>
#include "Widget_TextLabel.h"

class URHO3D_API Input_Label : public IoComponentBase {
	URHO3D_OBJECT(Input_Label, IoComponentBase)
public:
	Input_Label(Urho3D::Context* context);

	virtual Urho3D::String GetNodeStyle();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);


	static Urho3D::String iconTexture;
	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);
	void HandleLineEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};