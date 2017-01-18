#pragma once

#include "IoComponentBase.h"

class URHO3D_API Mesh_ClosestPoint : public IoComponentBase {
	URHO3D_OBJECT(Mesh_ClosestPoint, IoComponentBase)
public:
	Mesh_ClosestPoint(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	void AddInputSlot() = delete;
	void AddOutputSlot() = delete;
	void DeleteInputSlot(int index) = delete;
	void DeleteOutputSlot(int index) = delete;

	static Urho3D::String iconTexture;
};