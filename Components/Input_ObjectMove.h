#pragma once

#include "IoComponentBase.h"

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Camera.h>

class URHO3D_API Input_ObjectMove : public IoComponentBase {

	URHO3D_OBJECT(Input_ObjectMove, IoComponentBase)

public:
	Input_ObjectMove(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	Urho3D::Node* currentNode = NULL;
	Urho3D::Camera* currentCamera;
	Urho3D::Vector3 orgPos;
	Urho3D::Quaternion orgRot;
	Urho3D::Vector3 orgHitPoint;

	Urho3D::Vector<int> objectFilter;
	int constraintFlags = 7;
	int nodeID;
	Urho3D::Matrix3x4 transform;

	bool DoRaycast();

	void HandleMouseDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleMouseUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	Urho3D::Vector3 GetConstrainedVector(Urho3D::Vector3 moveVec, int flags);
};