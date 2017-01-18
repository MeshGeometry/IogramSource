#pragma once

#include "IoComponentBase.h"

class URHO3D_API Mesh_ExtrudePolyline : public IoComponentBase {

	URHO3D_OBJECT(Mesh_ExtrudePolyline, IoComponentBase)

public:
	Mesh_ExtrudePolyline(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	void AddInputSlot() = delete;
	void AddOutputSlot() = delete;
	void DeleteInputSlot(int index) = delete;
	void DeleteOutputSlot(int index) = delete;
};