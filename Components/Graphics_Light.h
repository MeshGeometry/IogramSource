#pragma once

#include <Urho3D/Core/Variant.h>

#include "IoComponentBase.h"

class URHO3D_API Graphics_Light : public IoComponentBase {
	URHO3D_OBJECT(Graphics_Light, IoComponentBase)
public:
	Graphics_Light(Urho3D::Context* context);

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);


	static Urho3D::String iconTexture;

	Urho3D::Vector<int> trackedItems_;
};