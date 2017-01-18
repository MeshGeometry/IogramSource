#pragma once

#include "IoComponentBase.h"

class URHO3D_API Input_ScanDir : public IoComponentBase {

	URHO3D_OBJECT(Input_ScanDir, IoComponentBase)

public:
	Input_ScanDir(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

};