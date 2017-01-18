#pragma once

#include "IoComponentBase.h"
#include <Urho3D/UI/UIElement.h>
#include "Widget_Container.h"

class URHO3D_API Input_ScreenContainer : public IoComponentBase {
	URHO3D_OBJECT(Input_ScreenContainer, IoComponentBase)
public:
	Input_ScreenContainer(Urho3D::Context* context);

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	Urho3D::Vector<Widget_Container*> trackedItems;
	static Urho3D::String iconTexture;
};