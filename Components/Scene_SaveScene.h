#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_SaveScene : public IoComponentBase {
	URHO3D_OBJECT(Scene_SaveScene, IoComponentBase)
public:
	Scene_SaveScene(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	static Urho3D::String iconTexture;

};