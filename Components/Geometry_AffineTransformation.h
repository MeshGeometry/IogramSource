#pragma once

#include "IoComponentBase.h"

class URHO3D_API Geometry_AffineTransformation : public IoComponentBase {

	URHO3D_OBJECT(Geometry_AffineTransformation, IoComponentBase)

public:
	Geometry_AffineTransformation(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

};