#pragma once

#include "IoComponentBase.h"

class URHO3D_API Mesh_Icosahedron : public IoComponentBase {
	URHO3D_OBJECT(Mesh_Icosahedron, IoComponentBase)
public:
	Mesh_Icosahedron(Urho3D::Context* context);

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