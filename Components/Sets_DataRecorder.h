#pragma once

#include "IoComponentBase.h"

class URHO3D_API Sets_DataRecorder : public IoComponentBase {

	URHO3D_OBJECT(Sets_DataRecorder, IoComponentBase)

public:
	Sets_DataRecorder(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	static Urho3D::String iconTexture;

	Urho3D::VariantVector trackedItems;
};