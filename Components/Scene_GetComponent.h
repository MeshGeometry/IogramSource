#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_GetComponent : public IoComponentBase {
	URHO3D_OBJECT(Scene_GetComponent, IoComponentBase)
public:
	Scene_GetComponent(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	static Urho3D::String iconTexture;
};