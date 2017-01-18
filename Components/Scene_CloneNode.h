#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_CloneNode : public IoComponentBase {

	URHO3D_OBJECT(Scene_CloneNode, IoComponentBase)

public:
	Scene_CloneNode(Urho3D::Context* context);
	~Scene_CloneNode();
	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	virtual void PreLocalSolve();

	Urho3D::Vector<int> trackedNodes;
};