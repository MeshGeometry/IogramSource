#pragma once

#include "IoComponentBase.h"

class URHO3D_API Input_StringReplace : public IoComponentBase {

	URHO3D_OBJECT(Input_StringReplace, IoComponentBase)

public:
	Input_StringReplace(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

};