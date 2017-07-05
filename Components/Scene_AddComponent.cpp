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


#include "Scene_AddComponent.h"
#include <Urho3D/Scene/Component.h>

using namespace Urho3D;

String Scene_AddComponent::iconTexture = "Textures/Icons/Scene_AddComponent.png";

Scene_AddComponent::Scene_AddComponent(Context* context) : IoComponentBase(context, 0, 0)
{
	//set up component info
	SetName("AddComponent");
	SetFullName("Add Component");
	SetDescription("Adds a native component to a scene node.");

	//set up the slots
	AddInputSlot(
		"Node ID",
		"ID",
		"Node ID to add component",
		VAR_INT,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Type",
		"T",
		"Type of component to add.",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Reference",
		"Ptr",
		"Reference to added component",
		VAR_PTR,
		DataAccess::ITEM
	);
}

void Scene_AddComponent::PreLocalSolve()
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedItems.Size(); i++)
		{
			Component* sm = scene->GetComponent(trackedItems[i]);
			if (sm != NULL)
			{
				scene->GetComponent(trackedItems[i])->Remove();
			}
		}
	}

	trackedItems.Clear();
}

//work function
void Scene_AddComponent::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered!");
		outSolveInstance[0] = Variant();
		return;
	}

	//otherwise proceed with finding the node
	int nodeID = inSolveInstance[0].GetInt();
	String compTypeName = inSolveInstance[1].GetString();

	Node* node = scene->GetNode(nodeID);
	if (node && nodeID > 0)
	{
		Component* comp = node->CreateComponent(compTypeName);
		if (comp)
		{
			trackedItems.Push(comp->GetID());
			outSolveInstance[0] = Variant(comp);
		}
		else
		{
			URHO3D_LOGERROR("Could not create component: " + compTypeName);
			outSolveInstance[0] = Variant();
		}
	}
	else
	{
		URHO3D_LOGERROR("Could not find node: " + String(nodeID));
		outSolveInstance[0] = Variant();
	}
}