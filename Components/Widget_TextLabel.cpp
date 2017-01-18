#include "Widget_TextLabel.h"
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>

// we will need a SendClickEvent

using namespace Urho3D;

Widget_TextLabel::Widget_TextLabel(Urho3D::Context* context) :Widget_Base(context)
{
	URHO3D_COPY_BASE_ATTRIBUTES(Widget_Base);
}

void Widget_TextLabel::CustomInterface(int size)
{
	String defaultText = "Double-click to edit";

	labelToDisplay = (Text*)GetChild("LabelText", false);
	if (labelToDisplay) {
		SetLabel(defaultText);
		SetLabelSize(size);
	}
	labelLineEdit = (LineEdit*)GetChild("LabelLineEdit", false);
	labelLineEdit->SetVisible(false);


	SubscribeToEvent(this, E_DOUBLECLICK, URHO3D_HANDLER(Widget_TextLabel, HandleDoubleClick));
	//SubscribeToEvent(this, E_DRAGMOVE, URHO3D_HANDLER(Widget_TextLabel, HandleBaseMove));

}

Widget_TextLabel::~Widget_TextLabel()
{
}

void Widget_TextLabel::SetLabel(String label)
{
	labelToDisplay->SetText(label);
}

void Widget_TextLabel::SetLabelSize(int size)
{
	labelToDisplay->SetFontSize(size);
}

void Widget_TextLabel::SetMovable(bool move)
{
	isMovable = move;
}


void Widget_TextLabel::HandleLineEdit(StringHash eventType, VariantMap& eventData)
{
	using namespace TextChanged;

	LineEdit* l = (LineEdit*)GetChild("LabelLineEdit", false);
	Text* curLabel = (Text*)GetChild("LabelText", false);
	if (l) {
		String labelString = l->GetText();
		SetLabel(labelString);
	}
	curLabel->SetVisible(true);
	l->SetVisible(false);
	isMovable = true;
}

// On double click, display editable label text in the line edit
void Widget_TextLabel::HandleDoubleClick(StringHash eventType, VariantMap& eventData)
{
	isMovable = false;
	LineEdit* curLineEdit = (LineEdit*)GetChild("LabelLineEdit", false);

	Text* curLabel = (Text*)GetChild("LabelText", false);
	curLabel->SetVisible(false);
	curLineEdit->SetVisible(true);
	curLineEdit->SetText(labelToDisplay->GetText());
	SubscribeToEvent(curLineEdit, E_TEXTFINISHED, URHO3D_HANDLER(Widget_TextLabel, HandleLineEdit));


}

// The label should be movable when not editing
void Widget_TextLabel::HandleBaseMove(StringHash eventType, VariantMap& eventData)
{
	using namespace DragMove;

	if (isMovable)
	{
		int DX = eventData[P_DX].GetInt();
		int DY = eventData[P_DY].GetInt();

		IntVector2 pos = GetPosition();
		SetPosition(pos + IntVector2(DX, DY));
	}

}
