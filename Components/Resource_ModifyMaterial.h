#pragma once

#include "IoComponentBase.h"

class URHO3D_API Resource_ModifyMaterial : public IoComponentBase {

	URHO3D_OBJECT(Resource_ModifyMaterial, IoComponentBase)

public:
	Resource_ModifyMaterial(Urho3D::Context* context);
public:

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;
};