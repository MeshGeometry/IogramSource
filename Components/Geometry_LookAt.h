#pragma once

#include "IoComponentBase.h"

class URHO3D_API Geometry_LookAt : public IoComponentBase {

	URHO3D_OBJECT(Geometry_LookAt, IoComponentBase)

public:
	Geometry_LookAt(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

};