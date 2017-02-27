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


#include "Input_Float.h"

#include "IoGraph.h"

#include <Urho3D/UI/UIEvents.h>

#include <assert.h>

using namespace Urho3D;

String Input_Float::iconTexture = "Textures/Icons/Input_Float.png";

Input_Float::Input_Float(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("Float");
	SetFullName("Float");
	SetDescription("Creates or casts a float");


	AddInputSlot(
		"Number",
		"X",
		"Non-float number",
		VAR_NONE,
		DataAccess::ITEM
		);

	AddOutputSlot(
		"Float",
		"F",
		"Float",
		VAR_VARIANTMAP,
		DataAccess::ITEM
		);

}

String Input_Float::GetNodeStyle()
{
	return "InputFloat";
}

void Input_Float::HandleCustomInterface(UIElement* customElement)
{
	floatNameEdit = (LineEdit*)customElement->GetChild("PropertyEdit", true);
	if (floatNameEdit)
	{
		SubscribeToEvent(floatNameEdit, E_TEXTFINISHED, URHO3D_HANDLER(Input_Float, HandleLineEdit));
        floatName = GetGenericData("FloatName").GetString();
        floatNameEdit->SetText(floatName);
	}
}

void Input_Float::HandleLineEdit(StringHash eventType, VariantMap& eventData)
{
	using namespace TextFinished;

	LineEdit* l = (LineEdit*)eventData[P_ELEMENT].GetVoidPtr();
	if (l)
	{

		Vector<int> path;
		path.Push(0);
		IoDataTree tree(GetContext());


		String val = l->GetText();
		Variant var(VAR_FLOAT, val);
        SetGenericData("FloatName", val);

		tree.Add(path, var);

		InputHardSet(0, tree);

		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}
}



void Input_Float::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{

	float in_var = inSolveInstance[0].GetFloat();

	outSolveInstance[0] = Variant(in_var);
}
