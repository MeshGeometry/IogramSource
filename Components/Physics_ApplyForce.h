#pragma once

#include <Urho3D/Core/Variant.h>
#include "IoComponentBase.h"

class URHO3D_API Physics_ApplyForce : public IoComponentBase {

	URHO3D_OBJECT(Physics_ApplyForce, IoComponentBase)

public:
	Physics_ApplyForce(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

};