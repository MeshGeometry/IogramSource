#pragma once
#include <Urho3D/UI/Button.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>

class URHO3D_API Widget_Base : public Urho3D::Button {

	URHO3D_OBJECT(Widget_Base, Urho3D::Button)

public:
	Widget_Base(Urho3D::Context* context);
	~Widget_Base() {};
	void HandleResize(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};