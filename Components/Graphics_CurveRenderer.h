#pragma once

#include "IoComponentBase.h"

class URHO3D_API Graphics_CurveRenderer : public IoComponentBase {

	URHO3D_OBJECT(Graphics_CurveRenderer, IoComponentBase)

public:
	Graphics_CurveRenderer(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	Urho3D::Vector<int> trackedItems;

};