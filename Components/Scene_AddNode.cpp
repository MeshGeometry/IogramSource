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


#include "Scene_AddNode.h"

#include "IoGraph.h"

#include "Geomlib_ConstructTransform.h"
#include "TransformEdit.h"

using namespace Urho3D;

String Scene_AddNode::iconTexture = "Textures/Icons/Scene_AddNode.png";

Scene_AddNode::Scene_AddNode(Urho3D::Context* context) : IoComponentBase(context, 4, 2)
{
	SetName("Add Node");
	SetFullName("Add Node To Scene");
	SetDescription("Adds a node with optional name and parent");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("");

	inputSlots_[0]->SetName("Transform");
	inputSlots_[0]->SetVariableName("T");
	inputSlots_[0]->SetDescription("Transform of node");
	inputSlots_[0]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Matrix3x4::IDENTITY);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Name");
	inputSlots_[1]->SetVariableName("N");
	inputSlots_[1]->SetDescription("Optional node name");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue("NewGeneratedNode");
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Parent");
	inputSlots_[2]->SetVariableName("P");
	inputSlots_[2]->SetDescription("Optional node parent");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(-1);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("Options");
	inputSlots_[3]->SetVariableName("O");
	inputSlots_[3]->SetDescription("Options to control node editing");
	inputSlots_[3]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(7);
	inputSlots_[3]->DefaultSet();


	outputSlots_[0]->SetName("Node ID");
	outputSlots_[0]->SetVariableName("ID");
	outputSlots_[0]->SetDescription("ID of node");
	outputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Transform");
	outputSlots_[1]->SetVariableName("T");
	outputSlots_[1]->SetDescription("Transform");
	outputSlots_[1]->SetVariantType(VariantType::VAR_MATRIX3X4);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	SubscribeToEvent("TransformChanged", URHO3D_HANDLER(Scene_AddNode, HandleTransformChanged));
}

Scene_AddNode::~Scene_AddNode()
{
	PreLocalSolve();
}

void Scene_AddNode::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	int flags = inSolveInstance[3].GetInt();

	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered.");
		return;
	}


	//add new node
	String name = inSolveInstance[1].GetString();

	Node* newNode = scene->CreateChild(name);

	//set parent if applicable
	int parentID = inSolveInstance[2].GetInt();
	Node* parentNode = scene->GetNode(parentID);
	if (parentNode)
	{
		newNode->SetParent(parentNode);
	}

	// construct transform:
	Matrix3x4 xform = Geomlib::ConstructTransform(inSolveInstance[0]);

	//apply transform	
	newNode->SetPosition(xform.Translation());
	newNode->SetRotation(xform.Rotation());
	newNode->SetScale(xform.Scale());

	//create transform edit
	if (flags != 0)
		TransformEdit* te = newNode->CreateComponent<TransformEdit>();

	//track
	trackedNodes.Push(newNode->GetID());

	outSolveInstance[0] = newNode->GetID();
}

void Scene_AddNode::PreLocalSolve()
{
	//delete old nodes
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedNodes.Size(); i++)
		{
			Node* oldNode = scene->GetNode(trackedNodes[i]);
			if (oldNode)
			{
				oldNode->RemoveAllChildren();
				oldNode->Remove();
			}

		}

		trackedNodes.Clear();
	}

}

void Scene_AddNode::HandleTransformChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	Node* node = (Node*)eventData["NodePtr"].GetPtr();
	if (node)
	{
		int id = node->GetID();
		if (trackedNodes.Contains(id))
		{
			Matrix3x4 xform = node->GetWorldTransform();

			outputSlots_[1]->SetIoDataTree(IoDataTree(GetContext(), xform));

			GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
		}
	}
}


