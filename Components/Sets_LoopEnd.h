#pragma once

#include "IoComponentBase.h"

class URHO3D_API Sets_LoopEnd : public IoComponentBase {

	URHO3D_OBJECT(Sets_LoopEnd, IoComponentBase)

public:
	Sets_LoopEnd(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	static Urho3D::String iconTexture;
};