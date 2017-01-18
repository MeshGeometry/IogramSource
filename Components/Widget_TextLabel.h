#pragma once
#include "Widget_Base.h"

#include <Urho3D/UI/LineEdit.h>

class URHO3D_API Widget_TextLabel : public Widget_Base {

	URHO3D_OBJECT(Widget_TextLabel, Widget_Base)

public:
	Widget_TextLabel(Urho3D::Context* context);
	~Widget_TextLabel();
	void CustomInterface(int size = 18);
	void SetLabel(Urho3D::String label);
	bool isMovable = true;
	void SetLabelSize(int size); // default to 18
	void SetMovable(bool move);
	Urho3D::LineEdit* GetLineEdit() { return labelLineEdit; };

private:

	Urho3D::Text* labelToDisplay;
	//SharedPtr<LineEdit> labelLineEdit;
	Urho3D::LineEdit* labelLineEdit;

	void HandleLineEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleDoubleClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleBaseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

};