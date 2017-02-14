#pragma once

#include "IoComponentBase.h"

class URHO3D_API ShapeOp_MeshTriangleStrain : public IoComponentBase {
	URHO3D_OBJECT(ShapeOp_MeshTriangleStrain, IoComponentBase)
public:
	ShapeOp_MeshTriangleStrain(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;
};