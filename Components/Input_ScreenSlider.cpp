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


#include <Urho3D/UI/Slider.h>

#include "Input_ScreenSlider.h"
#include "Widget_OptionSlider.h"

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "IoGraph.h"

using namespace Urho3D;

String Input_ScreenSlider::iconTexture = "Textures/Icons/Input_ScreenSlider.png";

Input_ScreenSlider::Input_ScreenSlider(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("ScreenSlider");
	SetFullName("Screen Slider");
	SetDescription("Adds a slider to the user interface");
	SetGroup(IoComponentGroup::USER);
	SetSubgroup("INPUT");

	AddInputSlot(
		"Position",
		"P",
		"Optional position. If left blank, the element is automatically positioned.",
		VAR_VECTOR3,
		ITEM,
		Variant()
	);

	AddInputSlot(
		"Range",
		"R",
		"Range of slider",
		VAR_VECTOR3,
		ITEM,
		Vector3(0, 50, 0)
	);

	AddInputSlot(
		"Label",
		"L",
		"Label",
		VAR_STRING,
		ITEM,
		"Slider Option: "
	);

	AddInputSlot(
		"Priority",
		"I",
		"Priority",
		VAR_INT,
		ITEM,
		0
	);

	AddInputSlot(
		"Parent",
		"PE",
		"Optional Parent element",
		VAR_PTR,
		ITEM
	);

	AddOutputSlot(
		"Screen Slider Out",
		"S",
		"Ptrs of Slider on Screen",
		VAR_INT,
		ITEM
	);
}

void Input_ScreenSlider::PreLocalSolve()
{
	//get the ui subsystem
	UI* ui = GetSubsystem<UI>();

	//delete all existing ui elements
	for (int i = 0; i < trackedItems.Size(); i++)
	{
		trackedItems[i]->SetEnabled(false);
		trackedItems[i]->Remove();

	}

	trackedItems.Clear();
}

void Input_ScreenSlider::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	//get the ui subsystem
	UI* ui = GetSubsystem<UI>();

	bool useCoords = false;
	if (inSolveInstance[0].GetType() == VAR_VECTOR3)
	{
		useCoords = true;
	}


	Vector3 range = inSolveInstance[1].GetVector3();
	String label = inSolveInstance[2].GetString();
	int idx = inSolveInstance[3].GetInt();
	UIElement* parent = (UIElement*)inSolveInstance[4].GetPtr();

	//get the active ui region
	UIElement* root = (UIElement*)GetGlobalVar("activeUIRegion").GetPtr();
	if (!root)
	{
		SetAllOutputsNull(outSolveInstance);
	}

	//attach
	UIElement* container = (parent == NULL) ? root : parent;

	//create the widget
	Widget_OptionSlider* optionSlider = container->CreateChild<Widget_OptionSlider>("CustomUI", idx);
	XMLFile* styleFile = GetSubsystem<ResourceCache>()->GetResource<XMLFile>("UI/IogramDefaultStyle.xml");
	optionSlider->SetStyle("Widget_OptionSlider", styleFile);
	optionSlider->CustomInterface();
	optionSlider->SetParams(range.x_, range.y_, range.x_);
	optionSlider->SetLabel(label);

	if (useCoords)
	{
		Vector3 coords = inSolveInstance[0].GetVector3();
		optionSlider->SetPosition(coords.x_, coords.y_);
	}


	Slider* slider = (Slider*)optionSlider->GetChild("MainSlider", false);
	trackedItems.Push(optionSlider);

	outSolveInstance[0] = (Slider*)slider;
}
