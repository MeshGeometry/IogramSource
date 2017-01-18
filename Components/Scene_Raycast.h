#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_Raycast : public IoComponentBase {

	URHO3D_OBJECT(Scene_Raycast, IoComponentBase)

public:
	Scene_Raycast(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

};