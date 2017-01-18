#pragma once

#include "IoComponentBase.h"

class URHO3D_API Geometry_Rotation : public IoComponentBase {

	URHO3D_OBJECT(Geometry_Rotation, IoComponentBase)

public:
	Geometry_Rotation(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

};