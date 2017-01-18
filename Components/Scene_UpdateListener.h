#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_UpdateListener : public IoComponentBase {

	URHO3D_OBJECT(Scene_UpdateListener, IoComponentBase)

public:
	Scene_UpdateListener(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	float lastStartTime = 0.0f;
	float elapsedTime = 0.0f;
	float deltaTime = 0.0f;
	bool isMuted = true;

	void HandleSceneUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

};