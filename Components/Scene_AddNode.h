#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_AddNode : public IoComponentBase {

	URHO3D_OBJECT(Scene_AddNode, IoComponentBase)

public:
	Scene_AddNode(Urho3D::Context* context);
	~Scene_AddNode();
	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	virtual void PreLocalSolve();

	Urho3D::Vector<int> trackedNodes;
};