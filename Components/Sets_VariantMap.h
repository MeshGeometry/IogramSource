#pragma once

#include "IoComponentBase.h"

class URHO3D_API Sets_VariantMap : public IoComponentBase {

	URHO3D_OBJECT(Sets_VariantMap, IoComponentBase)

public:
	Sets_VariantMap(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);
};