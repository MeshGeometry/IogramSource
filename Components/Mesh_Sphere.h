#pragma once

#include "IoComponentBase.h"

class URHO3D_API Mesh_Sphere : public IoComponentBase {
	URHO3D_OBJECT(Mesh_Sphere, IoComponentBase)
public:
	Mesh_Sphere(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	void AddInputSlot() = delete;
	void AddOutputSlot() = delete;
	void DeleteInputSlots(int index) = delete;
	void DeleteOutputSlot(int index) = delete;

	static Urho3D::String iconTexture;
};