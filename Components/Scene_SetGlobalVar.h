#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_SetGlobalVar : public IoComponentBase {
	URHO3D_OBJECT(Scene_SetGlobalVar, IoComponentBase)
public:
	Scene_SetGlobalVar(Urho3D::Context* context);

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance);

	static Urho3D::String iconTexture;

	Urho3D::Vector<Urho3D::String> trackedItems;
};