#pragma once

#include "IoComponentBase.h"

class URHO3D_API Mesh_SherkCollins : public IoComponentBase {
	URHO3D_OBJECT(Mesh_Torus, IoComponentBase)
public:
	Mesh_Torus(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;
};