#pragma once

#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/MultiLineEdit.h>

#include "IoComponentBase.h"

class URHO3D_API Input_Panel : public IoComponentBase {

	URHO3D_OBJECT(Input_Panel, IoComponentBase)

public:
	Input_Panel(Urho3D::Context* context);

public:

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;

	Urho3D::SharedPtr<Urho3D::MultiLineEdit> textArea_;

	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);
	bool editable_;

	void SetDataTree();
	void SetDataTreeContent();

	void HandleLineEditCommit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleGraphSolve(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleSetText(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleDefocus(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

};