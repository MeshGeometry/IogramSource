#pragma once

#include "IoComponentBase.h"

class URHO3D_API ShapeOp_Closeness : public IoComponentBase {
	URHO3D_OBJECT(ShapeOp_Closeness, IoComponentBase)
public:
	ShapeOp_Closeness(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	static Urho3D::String iconTexture;
};