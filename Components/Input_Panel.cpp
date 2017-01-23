#include "Input_Panel.h"
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/UIEvents.h>

#include <Urho3D/Graphics/Octree.h>
#include "IoGraph.h"

using namespace Urho3D;

String Input_Panel::iconTexture = "Textures/Icons/Input_Panel.png";

Input_Panel::Input_Panel(Urho3D::Context* context) :
	IoComponentBase(context, 1, 1),
	editable_(true)
{ 
	SubscribeToEvent("OnSolveGraph", URHO3D_HANDLER(Input_Panel, HandleGraphSolve));
}

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
	textArea_ = customElement->CreateChild<MultiLineEdit>("TextArea");
	textArea_->SetStyleAuto();
	textArea_->SetHeight(100);
	textArea_->GetTextElement()->SetFontSize(9);
	SubscribeToEvent(textArea_, E_TEXTFINISHED, URHO3D_HANDLER(Input_Panel, HandleLineEditCommit));
}

void Input_Panel::HandleGraphSolve(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	if (inputSlots_[0]->HasNoData())
	{
		editable_ = true;
		textArea_->SetEditable(true);
	}
	else
	{
		//try to set panel content
		IoDataTree* dt = inputSlots_[0]->GetIoDataTreePtr();
		String dtText = dt->ToString(false);

		if (textArea_.NotNull())
		{
			textArea_->SetText(dtText);
			textArea_->SetEditable(false);
		}

		editable_ = false;
	}
}

void Input_Panel::HandleLineEditCommit(StringHash eventType, VariantMap& eventData)
{
	using namespace TextFinished;

	if (textArea_.Null() || !editable_)
		return;

	//create a tree and data path
	Vector<int> path;
	path.Push(0);
	IoDataTree tree(GetContext());

	//split text in to multiple lines
	StringVector lines = textArea_->GetText().Split('\n');
	for (int i = 0; i < lines.Size(); i++)
	{

		String val = lines[i];

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

	//set input
	InputHardSet(0, tree);

	//call solve
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}



