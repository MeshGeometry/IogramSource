#pragma once

#include "IoComponentBase.h"

class URHO3D_API Mesh_Frame : public IoComponentBase {
	URHO3D_OBJECT(Mesh_Frame, IoComponentBase)
public:
	Mesh_Frame(Urho3D::Context* context);

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