#include "Input_Inspect.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/UI/ScrollView.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>

using namespace Urho3D;

String Input_Inspect::iconTexture = "Textures/Icons/Input_Inspect.png";

Input_Inspect::Input_Inspect(Urho3D::Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("Inspect");
	SetFullName("Inspect");
	SetDescription("Convert tree to string for inspection");
	SetGroup(IoComponentGroup::PARAMS);
	SetSubgroup("INPUT");

	inputSlots_[0]->SetName("TreeIn");
	inputSlots_[0]->SetVariableName("");
	inputSlots_[0]->SetDescription("Input Tree");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDefaultValue("");
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("TreeOut");
	outputSlots_[0]->SetVariableName("");
	outputSlots_[0]->SetDescription("Output Tree");
	outputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Input_Inspect::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	outSolveInstance[0] = inSolveInstance[0];
}

String Input_Inspect::GetNodeStyle()
{
	return "InspectNode";
}

void Input_Inspect::HandleCustomInterface(UIElement* customElement)
{
	ScrollView* sv = (ScrollView*)customElement->GetChild("ScrollView", false);
	Button* b = (Button*)customElement->GetChild("ContentPanel", true);
	if (sv && b)
	{
		sv->SetContentElement(b);
		treeText = b->CreateChild<Text>();
		treeText->SetStyleAuto();
		treeText->SetPosition(5, 5);
	}

	SubscribeToEvent("OnSolveGraph", URHO3D_HANDLER(Input_Inspect, HandleGraphSolve));
}

void Input_Inspect::HandleGraphSolve(StringHash eventType, VariantMap& eventData)
{
	IoDataTree tree = GetOutputIoDataTree(0);
	if (tree.IsEmptyTree()) {
		return;
	}
	if(treeText)
		treeText->SetText(tree.ToString());
}

