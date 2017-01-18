#pragma once

#include "IoComponentBase.h"

class URHO3D_API Maths_ConstructTransform : public IoComponentBase {

	URHO3D_OBJECT(Maths_ConstructTransform, IoComponentBase)

public:
	Maths_ConstructTransform(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

};