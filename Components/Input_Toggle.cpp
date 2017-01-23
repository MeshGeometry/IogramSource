#include "Input_Toggle.h"
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include "ColorDefs.h"

#include "IoGraph.h"

using namespace Urho3D;

String Input_Toggle::iconTexture = "Textures/Icons/Input_Toggle.png";

Input_Toggle::Input_Toggle(Urho3D::Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("Toggle");
	SetFullName("Boolean Toggle");
	SetDescription("Boolean Toggle input");
	SetGroup(IoComponentGroup::PARAMS);
	SetSubgroup("");

	inputSlots_[0]->SetName("");
	inputSlots_[0]->SetVariableName("");
	inputSlots_[0]->SetDescription("");
	inputSlots_[0]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(false);
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("");
	outputSlots_[0]->SetVariableName("");
	outputSlots_[0]->SetDescription("");
	outputSlots_[0]->SetVariantType(VariantType::VAR_BOOL); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

}

void Input_Toggle::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	outSolveInstance[0] = inSolveInstance[0];
}

String Input_Toggle::GetNodeStyle()
{
	return "BoolToggle";
}

void Input_Toggle::HandleCustomInterface(UIElement* customElement)
{
	Button* b = customElement->CreateChild<Button>("BoolToogle");
	b->SetStyle("Button");
	b->SetMinSize(30, 20);
	if (b)
	{
		SubscribeToEvent(b, E_PRESSED, URHO3D_HANDLER(Input_Toggle, HandleButtonPress));
	}
}

void Input_Toggle::HandleButtonPress(StringHash eventType, VariantMap& eventData)
{
	using namespace Pressed;
	Button* b = (Button*)eventData[P_ELEMENT].GetVoidPtr();
	if (b)
	{
		if (b->IsSelected())
		{
			b->SetSelected(false);
			b->SetColor(BRIGHT_ORANGE);

		}
		else
		{
			b->SetSelected(true);
			b->SetColor(LIGHT_GREEN);
		}

		Vector<int> path;
		path.Push(0);
		Variant var(b->IsSelected());
		IoDataTree tree(GetContext());
		tree.Add(path, var);
		InputHardSet(0, tree);

		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}
}