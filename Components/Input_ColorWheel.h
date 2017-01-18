#pragma once

#include "IoComponentBase.h"

class URHO3D_API Input_ColorWheel : public IoComponentBase {

	URHO3D_OBJECT(Input_ColorWheel, IoComponentBase)

public:
	Input_ColorWheel(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

};