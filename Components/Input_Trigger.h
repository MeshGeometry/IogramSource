#pragma once

#include "IoComponentBase.h"

class URHO3D_API Input_Trigger : public IoComponentBase {
	URHO3D_OBJECT(Input_Trigger, IoComponentBase)
public:
	Input_Trigger(Urho3D::Context* context);

	int LocalSolve();

	static Urho3D::String iconTexture;
};