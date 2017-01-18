#pragma once

#include "IoComponentBase.h"
#include <Urho3D/Graphics/RenderPath.h>

class URHO3D_API Scene_AppendRenderPath : public IoComponentBase {

	URHO3D_OBJECT(Scene_AppendRenderPath, IoComponentBase)

public:
	Scene_AppendRenderPath(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	virtual void PreLocalSolve();

	virtual void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	Urho3D::Vector<Urho3D::String> trackedItems;
};