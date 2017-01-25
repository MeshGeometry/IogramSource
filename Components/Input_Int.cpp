#include "Input_Int.h"

#include "IoGraph.h"

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>

#include <assert.h>

using namespace Urho3D;

String Input_Int::iconTexture = "Textures/Icons/Input_Integer.png";

Input_Int::Input_Int(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("Integer");
	SetFullName("Integer");
	SetDescription("Creates or casts an integer");


	AddInputSlot(
		"Number",
		"X",
		"Non-integer number",
		VAR_NONE,
		DataAccess::ITEM
		);

	AddOutputSlot(
		"Integer",
		"I",
		"Integer",
		VAR_VARIANTMAP,
		DataAccess::ITEM
		);

}

String Input_Int::GetNodeStyle()
{
	return "InputInteger";
}

void Input_Int::HandleCustomInterface(UIElement* customElement)
{
	intNameEdit = (LineEdit*)customElement->GetChild("PropertyEdit", true);
	if (intNameEdit)
	{
		SubscribeToEvent(intNameEdit, E_TEXTFINISHED, URHO3D_HANDLER(Input_Int, HandleLineEdit));
		intName = GetGenericData("IntName").GetString();
		intNameEdit->SetText(intName);
	}
}

void Input_Int::HandleLineEdit(StringHash eventType, VariantMap& eventData)
{
	using namespace TextFinished;

	LineEdit* l = (LineEdit*)eventData[P_ELEMENT].GetVoidPtr();
	if (l)
	{

		Vector<int> path;
		path.Push(0);
		IoDataTree tree(GetContext());

		
		String val = l->GetText();
		Variant var(VAR_INT, val);
        SetGenericData("IntName", val);

		tree.Add(path, var);
		
		InputHardSet(0, tree);

		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}
}



void Input_Int::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
		
	int in_var = inSolveInstance[0].GetInt();

	outSolveInstance[0] = Variant(in_var);

	//outSolveInstance[0] = inSolveInstance[0];
}
