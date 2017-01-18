#pragma once

#include "IoComponentBase.h"

class URHO3D_API Sets_ListLength : public IoComponentBase {
	URHO3D_OBJECT(Sets_ListLength, IoComponentBase)
public:
	Sets_ListLength(Urho3D::Context* context);

	int LocalSolve();

	static Urho3D::String iconTexture;
};