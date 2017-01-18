#pragma once

#include <Urho3D/Graphics/Model.h>

#include "IoComponentBase.h"

class URHO3D_API Graphics_CurveToModel : public IoComponentBase {

	URHO3D_OBJECT(Graphics_CurveToModel, IoComponentBase)

public:
	Graphics_CurveToModel(Urho3D::Context* context);
	~Graphics_CurveToModel();

	static Urho3D::String iconTexture;

	void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	Urho3D::Vector<Urho3D::SharedPtr<Urho3D::Model> > trackedItems;
	int autoNameCounter = 0;

};