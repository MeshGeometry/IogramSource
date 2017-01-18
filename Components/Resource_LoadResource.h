#pragma once

#include "IoComponentBase.h"

class URHO3D_API Resource_LoadResource: public IoComponentBase {

	URHO3D_OBJECT(Resource_LoadResource, IoComponentBase)

public:
	Resource_LoadResource(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	virtual void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);
};