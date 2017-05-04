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


#include "Input_Label.h"
#include "Widget_TextLabel.h"

#include <Urho3D/UI/UIEvents.h>

#include "IoGraph.h"

using namespace Urho3D;

String Input_Label::iconTexture = "Textures/Icons/DefaultIcon.png";

Input_Label::Input_Label(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("Label");
	SetFullName("Text Label");
	SetDescription("Label for annotating graphs");

	AddInputSlot(
		"Name",
		"N",
		"Name of this label",
		VAR_STRING,
		ITEM,
		"Default Text"
		);


	AddOutputSlot(
		"ContainerElement",
		"C",
		"Container Element",
		VAR_PTR,
		ITEM
		);

}

String Input_Label::GetNodeStyle() {
	 return "Component_TextLabel"; 
}

void Input_Label::HandleCustomInterface(UIElement* customElement) {

	Widget_TextLabel* myLabel = (Widget_TextLabel*)customElement;
	myLabel->SetStyle("Widget_TextLabel");
	myLabel->CustomInterface(12);
	String savedLabel = GetGenericData("labelText").GetString();
	if (savedLabel.Empty())
		savedLabel = "Double-click to edit";
	myLabel->SetLabel(savedLabel);
	myLabel->SetMovable(true);

	// subscribe to event line edit finish
	SubscribeToEvent(myLabel->GetLineEdit(), E_TEXTFINISHED, URHO3D_HANDLER(Input_Label, HandleLineEdit));
}

// ensure labels are saved to disk
void Input_Label::HandleLineEdit(StringHash eventType, VariantMap& eventData)
{
	using namespace TextFinished;
	String labelText = eventData[P_TEXT].GetString();
	SetGenericData("labelText", labelText);

}

void Input_Label::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	outSolveInstance[0] = inSolveInstance[0];
}
