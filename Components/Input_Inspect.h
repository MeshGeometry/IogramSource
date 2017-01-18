#pragma once

#include "IoComponentBase.h"
#include <Urho3D/UI/Text.h>

class URHO3D_API Input_Inspect : public IoComponentBase {

	URHO3D_OBJECT(Input_Inspect, IoComponentBase)

public:
	Input_Inspect(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	Urho3D::Text* treeText = NULL;

	virtual Urho3D::String GetNodeStyle();
	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);
	void HandleGraphSolve(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};