#pragma once
#include "Widget_Base.h"

#include <Urho3D/UI/ScrollView.h>
#include <Urho3D/UI/Text.h>

class URHO3D_API Widget_Container : public Widget_Base {

	URHO3D_OBJECT(Widget_Container, Widget_Base)

public:
	Widget_Container(Urho3D::Context* context);
	~Widget_Container();
	void CustomInterface();
	Urho3D::Button* GetContentElement() { return scrollContent; };
	Urho3D::ScrollView* GetScrollElement() { return scrollView; };
	Urho3D::Text* GetTextElement() { return label; };

	bool isMovable = false;
	bool useAutoLayout = true;
private:
	Urho3D::Button* scrollContent;
	Urho3D::ScrollView* scrollView;
	Urho3D::Text* label;

	void HandleBaseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleResize(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};