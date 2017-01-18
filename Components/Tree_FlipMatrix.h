#pragma once

#include "IoComponentBase.h"

class URHO3D_API Tree_FlipMatrix : public IoComponentBase {
	URHO3D_OBJECT(Tree_FlipMatrix, IoComponentBase)
public:
	Tree_FlipMatrix(Urho3D::Context* context);

	int LocalSolve();

	static Urho3D::String iconTexture;
};