#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_HandleEvent : public IoComponentBase {
	URHO3D_OBJECT(Scene_HandleEvent, IoComponentBase)
public:
	Scene_HandleEvent(Urho3D::Context* context);

	int LocalSolve();

	static Urho3D::String iconTexture;

	void GenericEventHandler(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};