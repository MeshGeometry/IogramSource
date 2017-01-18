#pragma once

#include <Urho3D/Core/Variant.h>

#include "IoComponentBase.h"

class URHO3D_API Vector_ColorPalette : public IoComponentBase {
	URHO3D_OBJECT(Vector_ColorPalette, IoComponentBase)
public:
	Vector_ColorPalette(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);


	static Urho3D::String iconTexture;
};