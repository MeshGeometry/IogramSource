#pragma once

#include "IoComponentBase.h"

class URHO3D_API ShapeOp_Gravity : public IoComponentBase {
	URHO3D_OBJECT(ShapeOp_Gravity, IoComponentBase)
public:
	ShapeOp_Gravity(Urho3D::Context* context);

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