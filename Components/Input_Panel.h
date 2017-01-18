#pragma once

#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/LineEdit.h>
#include "IoComponentBase.h"

class URHO3D_API Input_Panel : public IoComponentBase {

	URHO3D_OBJECT(Input_Panel, IoComponentBase)

public:
	Input_Panel(Urho3D::Context* context) : IoComponentBase(context, 1, 1) { };
public:

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;

	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);
	virtual Urho3D::String GetNodeStyle();

	Urho3D::UIElement* uiRoot = NULL;
	Urho3D::Button* addLine = NULL;
	Urho3D::Button* deleteLine = NULL;
	Urho3D::Vector<Urho3D::LineEdit*> lines;

	void HandleLineEditCommit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleNewLine(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleDeleteLine(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	Urho3D::LineEdit* CreateNewLine();
};