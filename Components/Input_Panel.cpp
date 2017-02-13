#include "Input_Panel.h"
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Input/InputEvents.h>
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

void Input_Panel::SetDataTree()
{
	//set saved data
	IoDataTree* dt = inputSlots_[0]->GetIoDataTreePtr();
	if (dt)
	{
		String dtText = dt->ToString(false);

		if (textArea_.NotNull())
		{
			textArea_->SetText(dtText);

			if (!inputSlots_[0]->GetLinkedOutputSlot())
			{
				//editable_ = true;
				//textArea_->SetEditable(true);
			}
			else
			{
				editable_ = false;
				textArea_->SetEditable(false);
			}
		}
	}

}

void Input_Panel::SetDataTreeContent()
{
	//set saved data
	IoDataTree* dt = inputSlots_[0]->GetIoDataTreePtr();
	if (dt && textArea_.NotNull())
	{
		Vector<String> contents = dt->GetContent();
		String flatContent = "";
		for (int i = 0; i < contents.Size(); i++)
		{
			flatContent += contents[i];
			
			if (!contents[i].EndsWith("\n"))
			{
				flatContent += "\n";
			}
		}

		textArea_->SetText(flatContent);
	}
}

void Input_Panel::HandleCustomInterface(UIElement* customElement)
{
	textArea_ = customElement->CreateChild<MultiLineEdit>("TextArea");
	textArea_->SetStyleAuto();
	textArea_->SetHeight(100);
	textArea_->GetTextElement()->SetFontSize(11);
	textArea_->SetEditable(false);
	textArea_->GetCursor()->SetMaxWidth(1);
	textArea_->GetCursor()->SetOpacity(0.7f);
	textArea_->GetCursor()->SetBlendMode(BLEND_ALPHA);

	targetType_ = customElement->CreateChild<LineEdit>("TargetType");
	targetType_->SetStyleAuto();

	String savedType = GetGenericData("TargetType").GetString();
	if (!savedType.Empty())
	{
		targetType_->SetText(savedType);
	}


	if (inputSlots_[0]->GetLinkedOutputSlot().NotNull())
		SetDataTree();
	else
		SetDataTreeContent();

	SubscribeToEvent(targetType_, E_TEXTFINISHED, URHO3D_HANDLER(Input_Panel, HandleTargetTypeChange));
	SubscribeToEvent(textArea_, E_TEXTFINISHED, URHO3D_HANDLER(Input_Panel, HandleLineEditCommit));
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Input_Panel, HandleSetText));
	SubscribeToEvent(E_DEFOCUSED, URHO3D_HANDLER(Input_Panel, HandleDefocus));
	SubscribeToEvent(E_FOCUSED, URHO3D_HANDLER(Input_Panel, HandleFocus));
}

void Input_Panel::HandleDefocus(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace Defocused;

	UIElement* el = (UIElement*)eventData[P_ELEMENT].GetPtr();
	if (el == textArea_)
	{
		if (textArea_.NotNull())
		{
			if (!textArea_->GetText().Empty())
			{

				VariantMap emptyMap;
				HandleLineEditCommit("", emptyMap);
			}

			textArea_->SetEditable(false);

		}
	}

	URHO3D_LOGINFO("Cursor width: " + String(textArea_->GetCursor()->GetWidth()));
}

void Input_Panel::HandleFocus(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace Defocused;

	UIElement* el = (UIElement*)eventData[P_ELEMENT].GetPtr();
	if (el == textArea_)
	{
		if (textArea_.NotNull())
		{
			textArea_->SetEditable(true);
		}

	}
}

void Input_Panel::HandleSetText(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace KeyDown;

	int key = eventData[P_KEY].GetInt();
	int qual = eventData[P_QUALIFIERS].GetInt();
	bool repeat = eventData[P_REPEAT].GetInt();

	if (key == KEY_RETURN && qual == QUAL_CTRL && !repeat)
	{
		if (textArea_.NotNull())
		{
			if (!textArea_->GetText().Empty())
			{

				VariantMap emptyMap;
				HandleLineEditCommit("", emptyMap);
			}

		}

	}
}

void Input_Panel::HandleGraphSolve(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	if (!inputSlots_[0]->GetLinkedOutputSlot())
	{
		//editable_ = true;
		if (textArea_.NotNull())
		{
			//textArea_->SetEditable(true);
			SetDataTreeContent();
		}
	}
	else
	{
		SetDataTree();
		//editable_ = false;
	}
}

void Input_Panel::HandleTargetTypeChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace TextFinished;

	if (targetType_.NotNull())
	{
		String newType = targetType_->GetText();
		SetGenericData("TargetType", newType);

		VariantMap emptyMap;
		HandleLineEditCommit("", emptyMap);
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

	String targetTypeName = "";
	if (targetType_.NotNull())
	{
		targetTypeName = targetType_->GetText();
	}

	if (!targetTypeName.Empty())
	{
		//split text in to multiple lines
		StringVector lines = textArea_->GetText().Split('\n');

		for (int i = 0; i < lines.Size(); i++)
		{

			String val = lines[i];
			Variant var(targetTypeName, val);

			if (var.GetType() != VAR_NONE)
			{
				tree.Add(path, var);
			}
		}


	}
	else
	{
		String currText = textArea_->GetText();
		tree.Add(path,currText);
	}

	//set input
	InputHardSet(0, tree);

	//call solve
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}

void Input_Panel::CastPanelContents()
{

}



