#pragma once

#include <Urho3D/Core/Variant.h>
#include "IoComponentBase.h"
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/LineEdit.h>

class URHO3D_API Sets_ImportViewData : public IoComponentBase {

	URHO3D_OBJECT(Sets_ImportViewData, IoComponentBase)

public:
	Sets_ImportViewData(Urho3D::Context* context);

	int LocalSolve();
	virtual Urho3D::String GetNodeStyle();
	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);

	static Urho3D::String iconTexture;

	Urho3D::LineEdit* importNameEdit;
	Urho3D::String importName;
	void HandleLineEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};