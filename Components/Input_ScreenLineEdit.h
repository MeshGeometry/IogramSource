#pragma once

#include <Urho3D/Core/Variant.h>
#include "IoComponentBase.h"
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/LineEdit.h>

class URHO3D_API Input_ScreenLineEdit : public IoComponentBase {

	URHO3D_OBJECT(Input_ScreenLineEdit, IoComponentBase)

public:
	Input_ScreenLineEdit(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	Urho3D::Vector<Urho3D::UIElement*> trackedItems;
};