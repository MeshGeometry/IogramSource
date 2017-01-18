#pragma once

#include "IoComponentBase.h"

#include <Urho3D/Graphics/Drawable.h>

class URHO3D_API Scene_SelectGeometry : public IoComponentBase {

	URHO3D_OBJECT(Scene_SelectGeometry, IoComponentBase)

public:
	Scene_SelectGeometry(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	void HandleMouseClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	bool isOn = false;
	Urho3D::Vector3 mPos;

	Urho3D::Vector<Urho3D::Drawable*> drawables;

};