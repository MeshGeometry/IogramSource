#pragma once

#include "IoComponentBase.h"
#include <Urho3D/Graphics/OctreeQuery.h>

class URHO3D_API Input_GeometryEdit : public IoComponentBase {

	URHO3D_OBJECT(Input_GeometryEdit, IoComponentBase)

public:
	Input_GeometryEdit(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	Urho3D::Vector<int> trackedItems;
	int constraintFlags = 7;
	bool reset = true;

	Urho3D::RayQueryResult currentHitResult;

	void HandleMouseDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleMouseUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	bool DoRaycast();
	int CreateEditGeometry(Urho3D::Variant geometry, float thickness, Urho3D::Color color);

	Urho3D::Vector3 GetConstrainedVector(Urho3D::Vector3 moveVec, int flags);
};