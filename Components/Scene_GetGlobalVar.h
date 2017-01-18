#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_GetGlobalVar : public IoComponentBase {
	URHO3D_OBJECT(Scene_GetGlobalVar, IoComponentBase)
public:
	Scene_GetGlobalVar(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance);

	static Urho3D::String iconTexture;
};