#pragma once

#include "IoComponentBase.h"

class URHO3D_API Sets_LoopBegin : public IoComponentBase {

	URHO3D_OBJECT(Sets_LoopBegin, IoComponentBase)

public:
	Sets_LoopBegin(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	int numSteps;
	int currentIndex;
	Urho3D::String loopID;
	Urho3D::Variant trackedData;

	static Urho3D::String iconTexture;

	void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};