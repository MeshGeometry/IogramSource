#pragma once

#include "IoComponentBase.h"
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/LineEdit.h>

class URHO3D_API Interop_ReceiveData : public IoComponentBase {

	URHO3D_OBJECT(Interop_ReceiveData, IoComponentBase)

public:
	Interop_ReceiveData(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	virtual Urho3D::String GetNodeStyle();
	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);
	static Urho3D::String iconTexture;

	Urho3D::VariantVector incomingData_;

	Urho3D::LineEdit* importPortEdit_;
	Urho3D::LineEdit* addressEdit_;

	int importPort_;
	Urho3D::String sourceAddress_;
	int MSG_CHAT = 32;

	void HandleLineEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};