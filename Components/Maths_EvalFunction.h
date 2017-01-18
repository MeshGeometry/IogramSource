#pragma once

#include "IoComponentBase.h"

class URHO3D_API Maths_EvalFunction : public IoComponentBase {

	URHO3D_OBJECT(Maths_EvalFunction, IoComponentBase)

public:
	Maths_EvalFunction(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);


};