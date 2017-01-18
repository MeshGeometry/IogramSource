#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_Text3D : public IoComponentBase {

	URHO3D_OBJECT(Scene_Text3D, IoComponentBase)

public:
	Scene_Text3D(Urho3D::Context* context);
	~Scene_Text3D() {};
	static Urho3D::String iconTexture;

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	Urho3D::Vector<int> trackedItems;
};