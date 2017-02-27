#pragma once

#include "IoComponentBase.h"

class URHO3D_API Tree_GetItem : public IoComponentBase {
	URHO3D_OBJECT(Tree_GetItem, IoComponentBase)
public:
	Tree_GetItem(Urho3D::Context* context);

	int LocalSolve();

//	void SolveInstance(
//		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
//		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
//	);

	static Urho3D::String iconTexture;
private:
	Urho3D::SharedPtr<IoDataTree> lookupTree;
};
