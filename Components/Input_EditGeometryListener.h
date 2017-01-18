#pragma once

#include "IoComponentBase.h"

class URHO3D_API Input_EditGeometryListener : public IoComponentBase {

	URHO3D_OBJECT(Input_EditGeometryListener, IoComponentBase)

public:
	Input_EditGeometryListener(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);
	Urho3D::Variant currentGeometry;
	void HandleEditGeometry(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};