#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Physics/CollisionShape.h>
#include "IoComponentBase.h"

class URHO3D_API Physics_CollisionShape : public IoComponentBase {

	URHO3D_OBJECT(Physics_CollisionShape, IoComponentBase)

public:
	Physics_CollisionShape(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	Urho3D::Vector<Urho3D::SharedPtr<Urho3D::CollisionShape> > trackedItems;

};