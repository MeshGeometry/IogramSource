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


#include "Scene_AnimatedModel.h"
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Material.h>

#include <Urho3D/Resource/ResourceCache.h>
#include "IoGraph.h"

using namespace Urho3D;

String Scene_AnimatedModel::iconTexture = "Textures/Icons/Scene_AnimatedModel.png";

Scene_AnimatedModel::Scene_AnimatedModel(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("Add Static Model");
	SetFullName("Add Static Model To Scene");
	SetDescription("Adds a static model");

	inputSlots_[0]->SetName("NodeID");
	inputSlots_[0]->SetVariableName("ID");
	inputSlots_[0]->SetDescription("ID of node");
	inputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("AnimatedModel");
	inputSlots_[1]->SetVariableName("AM");
	inputSlots_[1]->SetDescription("Pointer or path to Model");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue("Models/Jack.mdl");
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Material");
	inputSlots_[2]->SetVariableName("MT");
	inputSlots_[2]->SetDescription("Pointer to Material");
	inputSlots_[2]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue("Materials/PBR/Metallic3.xml");
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Animated Model");
	outputSlots_[0]->SetVariableName("AM");
	outputSlots_[0]->SetDescription("Pointer to animated Model");
	outputSlots_[0]->SetVariantType(VariantType::VAR_PTR); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Scene_AnimatedModel::PreLocalSolve()
{
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	if(scene)
	{
		for (int i = 0; i < trackedItems.Size(); i++)
		{
			Component* sm = scene->GetComponent(trackedItems[i]);
			if (sm != NULL)
			{
				sm->Remove();
			}
		}

		trackedItems.Clear();
	}
}

void Scene_AnimatedModel::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{

	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered.");
		outSolveInstance[0] = Variant();
		return;
	}

	int id = inSolveInstance[0].GetInt();
	Node* node = scene->GetNode(id);

	if (!node)
	{
		URHO3D_LOGERROR("Could not find node: " + String(id));
		outSolveInstance[0] = Variant();
		return;
	}

	Model* model = NULL;
	String type = inSolveInstance[1].GetTypeName();
	if (inSolveInstance[1].GetType() == VAR_PTR)
	{
		model = (Model*)inSolveInstance[1].GetPtr();
	}
	else if (inSolveInstance[1].GetType() == VAR_STRING)
	{
		model = cache->GetResource<Model>(inSolveInstance[1].GetString());
	}

	Material* mat = NULL;
	
	if (inSolveInstance[2].GetType() == VAR_STRING)
		mat = cache->GetResource<Material>(inSolveInstance[2].GetString());
	else if (inSolveInstance[2].GetType() == VAR_PTR)
		mat = (Material*)inSolveInstance[2].GetPtr();

	if (!model || !mat)
	{
		URHO3D_LOGERROR("Could not load model or material");
		outSolveInstance[0] = Variant();
		return;
	}

	AnimatedModel* am = node->CreateComponent<AnimatedModel>();
	am->SetModel(model->Clone());
	am->SetMaterial(mat->Clone());

	//also add the controller
	AnimationController* ac = node->CreateComponent<AnimationController>();

	trackedItems.Push(am->GetID());
	trackedItems.Push(ac->GetID());

	outSolveInstance[0] = ac;
}
