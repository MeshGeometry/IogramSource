#pragma once

#include "IoComponentBase.h"

class URHO3D_API Maths_MassAddition : public IoComponentBase {

	URHO3D_OBJECT(Maths_MassAddition, IoComponentBase)

public:
	static Urho3D::String iconTexture;
	Maths_MassAddition(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);
};