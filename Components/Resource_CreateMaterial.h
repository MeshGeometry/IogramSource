#pragma once

#include "IoComponentBase.h"

class URHO3D_API Resource_CreateMaterial : public IoComponentBase {

	URHO3D_OBJECT(Resource_CreateMaterial, IoComponentBase)

public:
	Resource_CreateMaterial(Urho3D::Context* context);
public:

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;
};