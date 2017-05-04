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


#include "Input_ScreenLineEdit.h"
#include "ColorDefs.h"
#include <vector>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "IoGraph.h"

using namespace Urho3D;

String Input_ScreenLineEdit::iconTexture = "Textures/Icons/Input_ScreenLineEdit.png";

Input_ScreenLineEdit::Input_ScreenLineEdit(Urho3D::Context* context) : IoComponentBase(context, 4, 1)
{
	SetName("ScreenLineEdit");
	SetFullName("Screen Line Edit");
	SetDescription("Adds a line edit to the user interface");
	SetGroup(IoComponentGroup::USER);
	SetSubgroup("INPUT");

	inputSlots_[0]->SetName("Screen Coords");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Point Coordinate in Screen Space");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant());
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Label");
	inputSlots_[1]->SetVariableName("L");
	inputSlots_[1]->SetDescription("Label");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue("My Text Input: ");
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Priority");
	inputSlots_[2]->SetVariableName("I");
	inputSlots_[2]->SetDescription("Optional priority index");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(0);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("Parent");
	inputSlots_[3]->SetVariableName("PE");
	inputSlots_[3]->SetDescription("Optional parent");
	inputSlots_[3]->SetVariantType(VariantType::VAR_PTR);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("Line Edit out");
	outputSlots_[0]->SetVariableName("LE");
	outputSlots_[0]->SetDescription("Ptr to line edit");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VOIDPTR); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

}


void Input_ScreenLineEdit::PreLocalSolve()
{
	//get the ui subsystem
	UI* ui = GetSubsystem<UI>();

	//delete all existing ui elements
	for (int i = 0; i < trackedItems.Size(); i++)
	{
		trackedItems[i]->Remove();
	}

	trackedItems.Clear();
}

void Input_ScreenLineEdit::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	//get the ui subsystem
	UI* ui = GetSubsystem<UI>();

	
	String label = inSolveInstance[1].GetString();
	int  idx = inSolveInstance[2].GetInt();
	UIElement* parent = (UIElement*)inSolveInstance[3].GetPtr();

	bool useCoords = false;
	if (inSolveInstance[0].GetType() == VAR_VECTOR3)
	{
		useCoords = true;
	}

	//get the active ui region
	UIElement* root = (UIElement*)GetGlobalVar("activeUIRegion").GetPtr();
	if (!root)
	{
		SetAllOutputsNull(outSolveInstance);
	}

	//attach
	UIElement* container = (parent == NULL) ? root : parent;

	Button* b = (Button*)container->CreateChild<Button>("CustomUI", idx);
	XMLFile* styleFile = GetSubsystem<ResourceCache>()->GetResource<XMLFile>("UI/IogramDefaultStyle.xml");
	b->SetStyle("ScreenLineEdit", styleFile);

	if (useCoords)
	{
		Vector3 coords = inSolveInstance[0].GetVector3();
		b->SetPosition(coords.x_, coords.y_);
	}

	Text* tLabel = (Text*)b->GetChild("EditLabel", false);
	if (tLabel) {
		tLabel->SetText(label);
	}

	trackedItems.Push(b);
	LineEdit* le = (LineEdit*)b->GetChild("LineEditOption", false);

	outSolveInstance[0] = le;
}
