#pragma once

#include "IoComponentBase.h"

class URHO3D_API Curve_PolylineEvaluate : public IoComponentBase {
	URHO3D_OBJECT(Curve_PolylineEvaluate, IoComponentBase)
public:
	Curve_PolylineEvaluate(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	static Urho3D::String iconTexture;
};