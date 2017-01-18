#pragma once

#include "IoComponentBase.h"

class URHO3D_API Tree_Graft : public IoComponentBase {
	URHO3D_OBJECT(Tree_Graft, IoComponentBase)
public:
	Tree_Graft(Urho3D::Context* context);

	int LocalSolve();

	static Urho3D::String iconTexture;
};
