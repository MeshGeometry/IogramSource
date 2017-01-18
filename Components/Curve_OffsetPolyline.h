#pragma once

#include "IoComponentBase.h"

class URHO3D_API Curve_OffsetPolyline : public IoComponentBase {

	URHO3D_OBJECT(Curve_OffsetPolyline, IoComponentBase)

public:
	Curve_OffsetPolyline(Urho3D::Context* context);
public:

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;
};
