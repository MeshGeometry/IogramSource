#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_ScreenPointToRay : public IoComponentBase {

	URHO3D_OBJECT(Scene_ScreenPointToRay, IoComponentBase)

public:
	Scene_ScreenPointToRay(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

};