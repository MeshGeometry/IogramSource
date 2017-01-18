#pragma once

#include "IoComponentBase.h"
#include <Urho3D/UI/UIElement.h>

class URHO3D_API Input_CustomElement : public IoComponentBase {
	URHO3D_OBJECT(Input_CustomElement, IoComponentBase)
public:
	Input_CustomElement(Urho3D::Context* context);

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	Urho3D::Vector<Urho3D::UIElement*> trackedItems;
	static Urho3D::String iconTexture;
};