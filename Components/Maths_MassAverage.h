#pragma once

#include "IoComponentBase.h"

class URHO3D_API Maths_MassAverage : public IoComponentBase {

	URHO3D_OBJECT(Maths_MassAverage, IoComponentBase)

public:
	static Urho3D::String iconTexture;
	Maths_MassAverage(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);
};