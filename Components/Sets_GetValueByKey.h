#pragma once

#include "IoComponentBase.h"

class URHO3D_API Sets_GetValueByKey : public IoComponentBase {

	URHO3D_OBJECT(Sets_GetValueByKey, IoComponentBase)

public:
	Sets_GetValueByKey(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	static Urho3D::String iconTexture;
};