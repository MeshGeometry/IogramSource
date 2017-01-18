#pragma once

#include <Urho3D/Core/Variant.h>

#include "IoComponentBase.h"

class URHO3D_API Vector_ClosestPoint : public IoComponentBase {
	URHO3D_OBJECT(Vector_ClosestPoint, IoComponentBase)
public:
	Vector_ClosestPoint(Urho3D::Context* context);

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