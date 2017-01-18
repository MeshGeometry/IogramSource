#pragma once

#include "IoComponentBase.h"
#include <Urho3D/Graphics/Viewport.h>


class URHO3D_API Graphics_Viewport : public IoComponentBase {

	URHO3D_OBJECT(Graphics_Viewport, IoComponentBase)

public:
	Graphics_Viewport(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	Urho3D::Vector<int> trackedItems;
};