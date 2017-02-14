#pragma once

#include "IoComponentBase.h"

class URHO3D_API ShapeOp_GenericConstraint : public IoComponentBase {
	URHO3D_OBJECT(ShapeOp_GenericConstraint, IoComponentBase)
public:
	ShapeOp_GenericConstraint(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;
};