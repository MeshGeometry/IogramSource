#include "Input_Label.h"
#include "Widget_TextLabel.h"

#include <Urho3D/UI/UIEvents.h>

#include "IoGraph.h"

using namespace Urho3D;

String Input_Label::iconTexture = "Textures/Icons/DefaultIcon.png";

Input_Label::Input_Label(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("Label");
	SetFullName("Text Label");
	SetDescription("Label for annotating graphs");

	AddInputSlot(
		"Name",
		"N",
		"Name of this label",
		VAR_STRING,
		ITEM,
		"Default Text"
		);


	AddOutputSlot(
		"ContainerElement",
		"C",
		"Container Element",
		VAR_PTR,
		ITEM
		);

}

String Input_Label::GetNodeStyle() {
	 return "Component_TextLabel"; 
}

void Input_Label::HandleCustomInterface(UIElement* customElement) {

	Widget_TextLabel* myLabel = (Widget_TextLabel*)customElement;
	myLabel->SetStyle("Widget_TextLabel");
	myLabel->CustomInterface(12);
	String savedLabel = GetGenericData("labelText").GetString();
	if (savedLabel.Empty())
		savedLabel = "Double-click to edit";
	myLabel->SetLabel(savedLabel);
	myLabel->SetMovable(true);

	// subscribe to event line edit finish
	SubscribeToEvent(myLabel->GetLineEdit(), E_TEXTFINISHED, URHO3D_HANDLER(Input_Label, HandleLineEdit));
}

// ensure labels are saved to disk
void Input_Label::HandleLineEdit(StringHash eventType, VariantMap& eventData)
{
	using namespace TextFinished;
	String labelText = eventData[P_TEXT].GetString();
	SetGenericData("labelText", labelText);

}

void Input_Label::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	outSolveInstance[0] = inSolveInstance[0];
}
