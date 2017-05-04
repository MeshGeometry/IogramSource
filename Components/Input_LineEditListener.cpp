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


#include "Input_LineEditListener.h"

#include <assert.h>

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/UI/LineEdit.h>

#include "IoGraph.h"

using namespace Urho3D;

String Input_LineEditListener::iconTexture = "Textures/Icons/Input_LineEditListener.png";


Input_LineEditListener::Input_LineEditListener(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("LineEditListener");
	SetFullName("Line Edit Listener");
	SetDescription("Listens for line edit input");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("INPUT");

	inputSlots_[0]->SetName("LineEdit Pointers");
	inputSlots_[0]->SetVariableName("LE");
	inputSlots_[0]->SetDescription("Pointers to screen line edit");
	inputSlots_[0]->SetVariantType(VariantType::VAR_PTR);

	inputSlots_[1]->SetName("Mute");
	inputSlots_[1]->SetVariableName("M");
	inputSlots_[1]->SetDescription("Mute this listener");
	inputSlots_[1]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[1]->SetDefaultValue(false);
	inputSlots_[1]->DefaultSet();


	outputSlots_[0]->SetName("Values");
	outputSlots_[0]->SetVariableName("V");
	outputSlots_[0]->SetDescription("Values Out");
	outputSlots_[0]->SetVariantType(VariantType::VAR_BOOL); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Input_LineEditListener::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{

	if (inSolveInstance[0].GetPtr() == NULL)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	LineEdit* le = (LineEdit*)inSolveInstance[0].GetPtr();
	String typeName = le->GetTypeName();
	bool mute = inSolveInstance[1].GetBool();
	if (le && typeName == LineEdit::GetTypeNameStatic())
	{
		if (!mute)
			SubscribeToEvent(le, E_TEXTFINISHED, URHO3D_HANDLER(Input_LineEditListener, HandleLineEdit));
		else
			UnsubscribeFromEvent(E_TEXTFINISHED);

		//cast the text if possible
		String val = le->GetText();
		VariantType type = VAR_STRING;

		//try to find type by splitting with comma
		Vector<String> parts = val.Split(',');
		if (parts.Size() > 1)
		{
			type = Variant::GetTypeFromName(parts[1].Trimmed());
		}

		Variant var(type, val);
		outSolveInstance[0] = var;
	}
}

void Input_LineEditListener::HandleLineEdit(StringHash eventType, VariantMap& eventData)
{
	solvedFlag_ = 0;
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}