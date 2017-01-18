#pragma once

#include "IoComponentBase.h"

class URHO3D_API Input_StringAppend : public IoComponentBase {

	URHO3D_OBJECT(Input_StringAppend, IoComponentBase)

public:
	Input_StringAppend(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

};