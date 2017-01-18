#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_SendEvent : public IoComponentBase {
	URHO3D_OBJECT(Scene_SendEvent, IoComponentBase)
public:
	Scene_SendEvent(Urho3D::Context* context);

	int LocalSolve();

	static Urho3D::String iconTexture;
};