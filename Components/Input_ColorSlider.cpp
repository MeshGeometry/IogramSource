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


#include "Input_ColorSlider.h"

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "IoGraph.h"
#include "ColorSlider.h"

using namespace Urho3D;

String Input_ColorSlider::iconTexture = "Textures/Icons/Input_ColorWheel.png";

Input_ColorSlider::Input_ColorSlider(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("Color");
	SetFullName("Color Selector");
	SetDescription("Slider for selecting a Color");

	AddInputSlot(
		"Color",
		"C",
		"Color",
		VAR_COLOR,
		ITEM
		);


	AddOutputSlot(
		"Color_out",
		"C",
		"Color output",
		VAR_PTR,
		ITEM
		);

}


void Input_ColorSlider::HandleCustomInterface(UIElement* customElement) {

	//remove header and inputs
	UIElement* parent = customElement->GetParent();
	UIElement* header = parent->GetChild("Header", false);
	UIElement* body = parent->GetChild("Body", false);


	mySlider = customElement->CreateChild<ColorSlider>();
	customElement->SetMinSize(120, 60);

	//try to get current color
	Color color = GetGenericData("Color").GetColor();
	mySlider->SetCurrentColor(color);

	PushChange(color, false);

	SubscribeToEvent(mySlider, "ColorSliderChanged", URHO3D_HANDLER(Input_ColorSlider, HandleSliderChanged));

}

void Input_ColorSlider::HandleSliderChanged(StringHash eventType, VariantMap& eventData)
{
	if (mySlider) {
		Color col = mySlider->GetCurrentColor();
		PushChange(col);
	}
}

void Input_ColorSlider::PushChange(Color curColor, bool solve)
{
	SetGenericData("Color", curColor);
	
    Vector<int> path;
	path.Push(0);
	Variant var(curColor);
	IoDataTree tree(GetContext());
	tree.Add(path, var);
	InputHardSet(0, tree);

	if (solve) {
		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}

}

void Input_ColorSlider::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	outSolveInstance[0] = inSolveInstance[0];

}
