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


#include "Input_Vector3.h"

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "IoGraph.h"
#include "MultiSlider.h"

using namespace Urho3D;

String Input_Vector3::iconTexture = "Textures/Icons/DefaultIcon.png";

Input_Vector3::Input_Vector3(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("Vector3");
	SetFullName("Vector3 Selector");
	SetDescription("Slider for selecting a Vector3");

	AddInputSlot(
		"Default",
		"N",
		"Vector3",
		VAR_VECTOR3,
		ITEM,
		Vector3::ZERO
		);


	AddOutputSlot(
		"Vector3",
		"V",
		"Vector output",
		VAR_PTR,
		ITEM
		);

}

String Input_Vector3::GetNodeStyle() {
	return "Component_Vector3Slider";
}

void Input_Vector3::HandleCustomInterface(UIElement* customElement) {

	customElement->SetMinSize(120, 60);
	mySlider = customElement->CreateChild<MultiSlider>();
	mySlider->AddSlider();
	mySlider->AddSlider();

	Vector3 lastVal = GetGenericData("Vector3").GetVector3();

	mySlider->SetVector3(lastVal);
	PushChange(lastVal, false);
	

	SubscribeToEvent(mySlider, "MultiSliderChanged", URHO3D_HANDLER(Input_Vector3, HandleSliderChanged));
}



void Input_Vector3::PushChange(Vector3 val, bool solve)
{
	
	SetGenericData("Vector3", val);
	
	
	Vector<int> path;
	path.Push(0);
	Variant var(val);
	IoDataTree tree(GetContext());
	tree.Add(path, var);
	InputHardSet(0, tree);
	if (solve) {
		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}

}

void Input_Vector3::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	outSolveInstance[0] = inSolveInstance[0];

}

void Input_Vector3::HandleSliderChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	if (mySlider) {
		Vector3 newValue = mySlider->GetVector3();
		PushChange(newValue);
	}

}
