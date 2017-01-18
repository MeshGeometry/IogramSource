#pragma once

#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>

#include "IoComponentBase.h"

class URHO3D_API Vector_DeconstructVector : public IoComponentBase {
	URHO3D_OBJECT(Vector_DeconstructVector, IoComponentBase)
public:
	Vector_DeconstructVector(Urho3D::Context* context);

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