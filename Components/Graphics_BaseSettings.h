#pragma once

#include "IoComponentBase.h"

class URHO3D_API Graphics_BaseSettings : public IoComponentBase {

	URHO3D_OBJECT(Graphics_BaseSettings, IoComponentBase)

public:
	Graphics_BaseSettings(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

};