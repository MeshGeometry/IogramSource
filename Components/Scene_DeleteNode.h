#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_DeleteNode : public IoComponentBase {
	URHO3D_OBJECT(Scene_DeleteNode, IoComponentBase)
public:
	Scene_DeleteNode(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	static Urho3D::String iconTexture;
};