#pragma once

#include "IoComponentBase.h"

class URHO3D_API ShapeOp_GeometryStrain : public IoComponentBase {
	URHO3D_OBJECT(ShapeOp_GeometryStrain, IoComponentBase)
public:
	ShapeOp_GeometryStrain(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;
};