#pragma once

#include "IoComponentBase.h"

class URHO3D_API Tree_Flatten : public IoComponentBase {
	URHO3D_OBJECT(Tree_Flatten, IoComponentBase)
public:
	Tree_Flatten(Urho3D::Context* context);

	int LocalSolve();

	static Urho3D::String iconTexture;
};
