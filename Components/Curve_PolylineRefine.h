#pragma once

#include "IoComponentBase.h"

class URHO3D_API Curve_PolylineRefine : public IoComponentBase {
	URHO3D_OBJECT(Curve_PolylineRefine, IoComponentBase)
public:
	Curve_PolylineRefine(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	static Urho3D::String iconTexture;
};