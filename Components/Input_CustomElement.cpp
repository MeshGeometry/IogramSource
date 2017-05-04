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


#include "Input_CustomElement.h"
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Input_CustomElement::iconTexture = "Textures/Icons/Input_CustomElement.png";

Input_CustomElement::Input_CustomElement(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("ScreenText");
	SetFullName("Screen Text");
	SetDescription("Adds some text to the screen UI");

	AddInputSlot(
		"CustomType",
		"T",
		"Custom type name to create. Name must be contained the style file",
		VAR_STRING,
		ITEM
	);

	AddInputSlot(
		"Style File",
		"F",
		"Optional file to look up. If blank, the default style file will be used.",
		VAR_STRING,
		ITEM,
		"UI/IogramDefaultStyle.xml"
	);

	AddInputSlot(
		"Position",
		"P",
		"Optional position. If left blank, the element is automatically positioned.",
		VAR_VECTOR3,
		ITEM,
		Variant()
	);

	AddInputSlot(
		"Size",
		"S",
		"Size of root element",
		VAR_VECTOR3,
		ITEM
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
		"CustomElement",
		"CE",
		"Custom Element",
		VAR_PTR,
		ITEM
	);

	AddOutputSlot(
		"Children",
		"C",
		"Children elements.",
		VAR_PTR,
		LIST
	);


}

void Input_CustomElement::PreLocalSolve()
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

void Input_CustomElement::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	//get the ui subsystem
	UI* ui = GetSubsystem<UI>();

	//get style name and reference file
	String styleName = inSolveInstance[0].GetString();
	String styleFile = inSolveInstance[1].GetString();
	UIElement* parent = (UIElement*)inSolveInstance[5].GetPtr();

	//attach to parent or root
	UIElement* container = (parent == NULL) ? ui->GetRoot() : parent;

	//exit if no style name
	if (styleName.Empty())
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//try to load a xml file.
	UIElement* customElement = NULL;
	if (styleFile.Contains(".xml"))
	{
		XMLFile* uiFile = GetSubsystem<ResourceCache>()->GetResource<XMLFile>(styleFile);
		XMLFile* elementFile = GetSubsystem<ResourceCache>()->GetResource<XMLFile>(styleName);
		if (!uiFile || !elementFile)
		{
			SetAllOutputsNull(outSolveInstance);
			return;
		}

		customElement = container->CreateChild<UIElement>("CustomUIElement");
		customElement->LoadXML(elementFile->GetRoot(), uiFile);

	}
	else
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//double check that proper ui element has been created
	if (!customElement)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//set the position and size if specified
	if (inSolveInstance[2].GetType() == VAR_VECTOR3)
	{
		Vector3 pos = inSolveInstance[2].GetVector3();
		customElement->SetPosition(pos.x_, pos.y_);
	}
	if (inSolveInstance[3].GetType() == VAR_VECTOR3)
	{
		Vector3 size = inSolveInstance[3].GetVector3();
		customElement->SetSize(size.x_, size.y_);
	}

	PODVector<UIElement*> children;
	customElement->GetChildren(children, true);

	VariantVector childVec;
	childVec.Resize(children.Size());
	for (int i = 0; i < children.Size(); i++)
	{
		childVec[i] = children[i];
	}

	//track and return
	trackedItems.Push(customElement);
	outSolveInstance[0] = customElement;
	outSolveInstance[1] = childVec;
}
