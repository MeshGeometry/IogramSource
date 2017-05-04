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


#include "Scene_ModifyNode.h"
#include <Urho3D/Scene/SceneEvents.h>

#include <assert.h>
#include <Urho3D/Core/Context.h>

using namespace Urho3D;

String Scene_ModifyNode::iconTexture = "Textures/Icons/Scene_ModifyNode.png";

Scene_ModifyNode::Scene_ModifyNode(Context* context) :
	IoComponentBase(context, 3, 2)
{
	SetName("ModifyNode");
	SetFullName("Modify Node");
	SetDescription("Modifies basic properties of a Node");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("ID");
	inputSlots_[0]->SetVariableName("ID");
	inputSlots_[0]->SetDescription("Node ID");
	inputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Transform");
	inputSlots_[1]->SetVariableName("T");
	inputSlots_[1]->SetDescription("New transfrom");
	inputSlots_[1]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[2]->SetName("Name");
	inputSlots_[2]->SetVariableName("N");
	inputSlots_[2]->SetDescription("Name");
	inputSlots_[2]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue("GeneratedNode");
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("ID");
	outputSlots_[0]->SetVariableName("ID");
	outputSlots_[0]->SetDescription("ID of node");
	outputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Scene_ModifyNode::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	if (!scene)
	{
		URHO3D_LOGERROR("No scene detected.");
		return;
	}

	Node* node = NULL;

	if (inSolveInstance[0].GetType() == VAR_INT)
	{
		node = scene->GetNode(inSolveInstance[0].GetInt());
	}

	else if (inSolveInstance[0].GetType() == VAR_STRING)
	{
		node = scene->GetChild(inSolveInstance[0].GetString(), true);
	}


	if (!node)
	{
		URHO3D_LOGERROR("No node with that id found.");
		return;
	}

	Matrix3x4 xform = inSolveInstance[1].GetMatrix3x4();
	String name = inSolveInstance[2].GetString();

	node->SetTransform(xform.Translation(), xform.Rotation(), xform.Scale());
	node->SetName(name);

	outSolveInstance[0] = node->GetID();
	outSolveInstance[1] = node->GetTransform();
}