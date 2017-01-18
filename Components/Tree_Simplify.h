#pragma once

#include "IoComponentBase.h"

class URHO3D_API Tree_Simplify : public IoComponentBase {
	URHO3D_OBJECT(Tree_Simplify, IoComponentBase)
public:
	Tree_Simplify(Urho3D::Context* context);

	int LocalSolve();

	static Urho3D::String iconTexture;
};
