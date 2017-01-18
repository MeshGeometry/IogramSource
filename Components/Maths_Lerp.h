#pragma once

#include "IoComponentBase.h"

class URHO3D_API Maths_Lerp : public IoComponentBase {

	URHO3D_OBJECT(Maths_Lerp, IoComponentBase)

public:
	Maths_Lerp(Urho3D::Context* context);
public:

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;
};