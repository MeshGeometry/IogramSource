//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "Input_MultiPanel.h"
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/UIEvents.h>

#include "IoGraph.h"

using namespace Urho3D;

String Input_MultiPanel::iconTexture = "";

void Input_MultiPanel::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	outSolveInstance[0] = inSolveInstance[0];
}

String Input_MultiPanel::GetNodeStyle()
{
	return "MultiPanel";
}

void Input_MultiPanel::HandleCustomInterface(UIElement* customElement)
{
	/*uiRoot = customElement;

	LineEdit* l = (LineEdit*)customElement->GetChild("PropertyEdit", true);
	if (l)
	{
		SubscribeToEvent(l, E_TEXTFINISHED, URHO3D_HANDLER(Input_Float, HandleLineEditCommit));
		lines.Push(l);
	}

	addLine = (Button*)customElement->GetChild("AddTextLine", true);
	if (addLine)
	{
		SubscribeToEvent(addLine, E_PRESSED, URHO3D_HANDLER(Input_Float, HandleNewLine));
	}

	deleteLine = (Button*)customElement->GetChild("DeleteTextLine", true);
	if (deleteLine)
	{
		SubscribeToEvent(deleteLine, E_PRESSED, URHO3D_HANDLER(Input_Float, HandleDeleteLine));
	}

	addCol = (Button*)customElement->GetChild("AddColumn", true);
	if (addCol)
	{
		SubscribeToEvent(addLine, E_PRESSED, URHO3D_HANDLER(Input_Float, HandleNewLine));
	}

	deleteCol = (Button*)customElement->GetChild("DeleteColumn", true);
	if (deleteCol)
	{
		SubscribeToEvent(deleteLine, E_PRESSED, URHO3D_HANDLER(Input_Float, HandleDeleteLine));
	}*/


	//SubscribeToEvent(customElement, E_RESIZED, URHO3D_HANDLER(Input_Float, HandleResize));

}


//void Input_Float::HandleLineEditCommit(StringHash eventType, VariantMap& eventData)
//{
//	using namespace TextFinished;
//
//	LineEdit* l = (LineEdit*)eventData[P_ELEMENT].GetVoidPtr();
//	if (l)
//	{
//
//		Vector<int> path;
//		path.Push(0);
//		IoDataTree tree(GetContext());
//
//		for (int i = 0; i < lines.Size(); i++)
//		{
//			String val = lines[i]->GetText();
//
//			VariantType type = VAR_STRING;
//
//			//try to find type by splitting with comma
//			Vector<String> parts = val.Split(',');
//			if (parts.Size() > 1)
//			{
//				type = Variant::GetTypeFromName(parts[1].Trimmed());
//			}
//
//			Variant var(type, val);
//
//			tree.Add(path, var);
//		}
//
//
//		InputHardSet(0, tree);
//
//		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
//	}
//}
//
//void Input_Float::HandleNewLine(StringHash eventType, VariantMap& eventData)
//{
//	if (!uiRoot)
//		return;
//
//	LineEdit* l = uiRoot->CreateChild<LineEdit>();
//	l->SetStyle("LineEdit");
//	l->SetPosition(5, lines.Size() * 32 + 5);
//	l->SetSize(uiRoot->GetSize().x_ - 10, 30);
//
//	SubscribeToEvent(l, E_TEXTFINISHED, URHO3D_HANDLER(Input_Float, HandleLineEditCommit));
//
//	lines.Push(l);
//
//	//resize ui root
//	uiRoot->SetSize(uiRoot->GetSize().x_, lines.Size() * 32 + 10 + 40);
//
//	//move adders
//	if (addLine && deleteLine)
//	{
//		addLine->SetPosition(addLine->GetPosition().x_, uiRoot->GetSize().y_ - 35);
//		deleteLine->SetPosition(deleteLine->GetPosition().x_, uiRoot->GetSize().y_ - 35);
//	}
//}
//
//void Input_Float::HandleDeleteLine(StringHash eventType, VariantMap& eventData)
//{
//	if (!uiRoot)
//		return;
//
//	if (lines.Size() <= 1)
//		return;
//
//	LineEdit* l = lines[lines.Size() - 1];
//	l->Remove();
//	lines.Remove(l);
//
//	//resize ui root
//	uiRoot->SetSize(uiRoot->GetSize().x_, lines.Size() * 32 + 10 + 40);
//
//	//move adders
//	if (addLine && deleteLine)
//	{
//		addLine->SetPosition(addLine->GetPosition().x_, uiRoot->GetSize().y_ - 35);
//		deleteLine->SetPosition(deleteLine->GetPosition().x_, uiRoot->GetSize().y_ - 35);
//	}
//
//	//reset the input
//	Vector<int> path;
//	path.Push(0);
//	IoDataTree tree(GetContext());
//
//	for (int i = 0; i < lines.Size(); i++)
//	{
//		String val = lines[i]->GetText();
//
//		VariantType type = VAR_STRING;
//
//		//try to find type by splitting with comma
//		Vector<String> parts = val.Split(',');
//		if (parts.Size() > 1)
//		{
//			type = Variant::GetTypeFromName(parts[1].Trimmed());
//		}
//
//		Variant var(type, val);
//
//		tree.Add(path, var);
//	}
//
//
//	InputHardSet(0, tree);
//
//	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
//
//}



//void Input_Float::HandleResize(StringHash eventType, VariantMap & eventData)
//{
//	using namespace Resized;
//	// these values should match the xml
//	int min_x = 80;
//	int min_y = 85;
//
//	// getting drag info
//	int X = eventData[P_WIDTH].GetInt();
//	int Y = eventData[P_HEIGHT].GetInt();
//	UIElement* n = (UIElement*)eventData[P_ELEMENT].GetPtr();
//
//	LineEdit* l = n->CreateChild<LineEdit>();
//	l->SetPosition(IntVector2(62, 32));
//
//}

