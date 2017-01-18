#pragma once

#include "IoComponentBase.h"

class URHO3D_API Scene_TriMeshVisualizeScalarField : public IoComponentBase {
	URHO3D_OBJECT(Scene_TriMeshVisualizeScalarField, IoComponentBase)
public:
	Scene_TriMeshVisualizeScalarField(Urho3D::Context* context);

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	void AddInputSlot() = delete;
	void AddOutputSlot() = delete;
	void DeleteInputSlots(int index) = delete;
	void DeleteOutputSlot(int index) = delete;

	static Urho3D::String iconTexture;

private:
	Urho3D::Vector<int> tracked_items;

#ifdef EMSCRIPTEN
	Urho3D::String normalMat = "Materials/BasicWeb.xml";
#endif

#ifndef EMSCRIPTEN
	Urho3D::String normalMat = "Materials/BasicPBR.xml";
#endif
};