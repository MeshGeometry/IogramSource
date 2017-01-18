#pragma once

#include <Urho3D/Core/Variant.h>

#include "IoComponentBase.h"

class URHO3D_API Spatial_ReadOSM : public IoComponentBase {
	URHO3D_OBJECT(Spatial_ReadOSM, IoComponentBase)
public:
	Spatial_ReadOSM(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);


	static Urho3D::String iconTexture;
};