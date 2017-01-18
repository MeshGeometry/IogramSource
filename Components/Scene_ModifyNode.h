#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_ModifyNode : public IoComponentBase {

	URHO3D_OBJECT(Scene_ModifyNode, IoComponentBase)

public:
	Scene_ModifyNode(Urho3D::Context* context);
public:

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;
};