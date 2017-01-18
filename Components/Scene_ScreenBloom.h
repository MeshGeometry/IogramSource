#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_ScreenBloom : public IoComponentBase {

	URHO3D_OBJECT(Scene_ScreenBloom, IoComponentBase)

public:
	Scene_ScreenBloom(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	bool hasBloom = false;
};