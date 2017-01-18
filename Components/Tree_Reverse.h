#pragma once

#include "IoComponentBase.h"

class URHO3D_API Tree_Reverse : public IoComponentBase {
	URHO3D_OBJECT(Tree_Reverse, IoComponentBase)
public:
	Tree_Reverse(Urho3D::Context* context);

	int LocalSolve();

	static Urho3D::String iconTexture;
};
