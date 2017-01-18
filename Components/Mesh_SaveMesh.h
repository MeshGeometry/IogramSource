#pragma once


#include "IoComponentBase.h"

class URHO3D_API Mesh_SaveMesh : public IoComponentBase {
	URHO3D_OBJECT(Mesh_SaveMesh, IoComponentBase)
public:
	Mesh_SaveMesh(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);


	static Urho3D::String iconTexture;
};