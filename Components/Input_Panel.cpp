#include "Input_Panel.h"
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/UIEvents.h>

#include <Urho3D/Graphics/Octree.h>
#include "IoGraph.h"

using namespace Urho3D;

String Input_Panel::iconTexture = "Textures/Icons/Input_Panel.png";

void Input_Panel::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	outSolveInstance[0] = inSolveInstance[0];
}

//TODO now that multiline edit is working, rework this component

void Input_Panel::HandleCustomInterface(UIElement* customElement)
{
	
	uiRoot = customElement;

	//LineEdit* l = (LineEdit*)customElement->GetChild("PropertyEdit", true);
	LineEdit* l = customElement->CreateChild<LineEdit>("PropertyEdit");
	l->SetStyle("LineEdit");
	if (l)
	{
		SubscribeToEvent(l, E_TEXTFINISHED, URHO3D_HANDLER(Input_Panel, HandleLineEditCommit));
		lines.Push(l);
	}

	addLine = (Button*)customElement->GetChild("AddTextLine", true);
	if (addLine)
	{
		SubscribeToEvent(addLine, E_PRESSED, URHO3D_HANDLER(Input_Panel, HandleNewLine));
	}

	deleteLine = (Button*)customElement->GetChild("DeleteTextLine", true);
	if (deleteLine)
	{
		SubscribeToEvent(deleteLine, E_PRESSED, URHO3D_HANDLER(Input_Panel, HandleDeleteLine));
	}

	//create text line edits from input slot data
	IoDataTree* tree = inputSlots_[0]->GetIoDataTreePtr();
	Vector<int> path = tree->Begin();
	//only iterate through one branch
	int numItems = tree->GetNumItemsAtBranch(path, DataAccess::ITEM);
	for (int i = 0; i < numItems; i++)
	{
		Variant var;
		tree->GetNextItem(var, DataAccess::ITEM);

		if (i == 0 && lines.Size() > 0)
		{
			l = lines.At(0);
			l->SetText(var.ToString());
		}
		else
		{
			l = CreateNewLine();
			l->SetText(var.ToString());
		}
	}


	UIElement* controls = customElement->CreateChild<UIElement>("ControlGroup");
	controls->SetMinHeight(30);
	controls->SetLayoutMode(LM_HORIZONTAL);
	controls->SetLayoutSpacing(2);
}

String Input_Panel::GetNodeStyle()
{
	return "TextLineNode";
}

void Input_Panel::HandleLineEditCommit(StringHash eventType, VariantMap& eventData)
{
	using namespace TextFinished;

	LineEdit* l = (LineEdit*)eventData[P_ELEMENT].GetVoidPtr();
	if (l)
	{
		
		Vector<int> path;
		path.Push(0);
		IoDataTree tree(GetContext());
		
		for (int i = 0; i < lines.Size(); i++)
		{
			String val = lines[i]->GetText();

			VariantType type = VAR_STRING;

			//try to find type by splitting with comma
			Vector<String> parts = val.Split(',');
			if (parts.Size() > 1)
			{
				type = Variant::GetTypeFromName(parts[1].Trimmed());
			}

			Variant var(type, val);
			
			tree.Add(path, var);
		}
		
		
		InputHardSet(0, tree);

		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}
}

LineEdit* Input_Panel::CreateNewLine()
{
	LineEdit* l = uiRoot->CreateChild<LineEdit>();
	l->SetStyle("LineEdit");
	l->SetPosition(5, lines.Size() * 32 + 5);
	l->SetSize(uiRoot->GetSize().x_ - 10, 30);

	SubscribeToEvent(l, E_TEXTFINISHED, URHO3D_HANDLER(Input_Panel, HandleLineEditCommit));

	lines.Push(l);

	//resize ui root
	uiRoot->SetSize(uiRoot->GetSize().x_, lines.Size() * 32 + 10 + 40);

	//move adders
	if (addLine && deleteLine)
	{
		addLine->SetPosition(addLine->GetPosition().x_, uiRoot->GetSize().y_ - 35);
		deleteLine->SetPosition(deleteLine->GetPosition().x_, uiRoot->GetSize().y_ - 35);
	}

	return l;

}

void Input_Panel::HandleNewLine(StringHash eventType, VariantMap& eventData)
{
	if (!uiRoot)
		return;

	CreateNewLine();

}

void Input_Panel::HandleDeleteLine(StringHash eventType, VariantMap& eventData)
{
	if (!uiRoot)
		return;

	if (lines.Size() <= 1)
		return;

	LineEdit* l = lines[lines.Size() - 1];
	l->Remove();
	lines.Remove(l);

	//resize ui root
	uiRoot->SetSize(uiRoot->GetSize().x_, lines.Size() * 32 + 10 + 40);

	//move adders
	if (addLine && deleteLine)
	{
		addLine->SetPosition(addLine->GetPosition().x_, uiRoot->GetSize().y_ - 35);
		deleteLine->SetPosition(deleteLine->GetPosition().x_, uiRoot->GetSize().y_ - 35);
	}

	//reset the input
	Vector<int> path;
	path.Push(0);
	IoDataTree tree(GetContext());

	for (int i = 0; i < lines.Size(); i++)
	{
		String val = lines[i]->GetText();

		VariantType type = VAR_STRING;

		//try to find type by splitting with comma
		Vector<String> parts = val.Split(',');
		if (parts.Size() > 1)
		{
			type = Variant::GetTypeFromName(parts[1].Trimmed());
		}

		Variant var(type, val);

		tree.Add(path, var);
	}


	InputHardSet(0, tree);

	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();

}

