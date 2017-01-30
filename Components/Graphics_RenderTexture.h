#pragma once

#include "IoComponentBase.h"
#include <Urho3D/Graphics/Viewport.h>


class URHO3D_API Graphics_RenderTexture : public IoComponentBase {

	URHO3D_OBJECT(Graphics_RenderTexture, IoComponentBase)

public:
	Graphics_RenderTexture(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	Urho3D::Vector<Urho3D::String> trackedItems;

	void HandleRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};