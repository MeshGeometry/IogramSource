#pragma once

#include "IoComponentBase.h"

class URHO3D_API Input_SliderListener : public IoComponentBase {
	URHO3D_OBJECT(Input_SliderListener, IoComponentBase)
public:
	Input_SliderListener(Urho3D::Context* context);
	virtual ~Input_SliderListener() {};

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;

	void HandleSliderChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& data);
};