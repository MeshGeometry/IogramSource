#pragma once

#include "IoComponentBase.h"
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/LineEdit.h>

class URHO3D_API Sets_ExportViewData : public IoComponentBase {

	URHO3D_OBJECT(Sets_ExportViewData, IoComponentBase)

public:
	Sets_ExportViewData(Urho3D::Context* context);

	int LocalSolve();
	virtual Urho3D::String GetNodeStyle();
	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);
	static Urho3D::String iconTexture;

	Urho3D::LineEdit* exportNameEdit;
	Urho3D::String exportName;
	void HandleLineEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};