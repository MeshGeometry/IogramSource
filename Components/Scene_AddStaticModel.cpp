#include "Scene_AddStaticModel.h"

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "IoGraph.h"

using namespace Urho3D;

String Scene_AddStaticModel::iconTexture = "Textures/Icons/Scene_AddStaticModel.png";

Scene_AddStaticModel::Scene_AddStaticModel(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("Add Static Model");
	SetFullName("Add Static Model To Scene");
	SetDescription("Adds a static model");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("");

	inputSlots_[0]->SetName("NodeID");
	inputSlots_[0]->SetVariableName("ID");
	inputSlots_[0]->SetDescription("ID of node");
	inputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Model");
	inputSlots_[1]->SetVariableName("MD");
	inputSlots_[1]->SetDescription("Pointer to Model");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue("Models/Box.mdl");
	inputSlots_[1]->DefaultSet();


	inputSlots_[2]->SetName("Material");
	inputSlots_[2]->SetVariableName("MT");
	inputSlots_[2]->SetDescription("Pointer to Material");
	inputSlots_[2]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue("Materials/PBR/BasicWeb.xml");
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Static Model");
	outputSlots_[0]->SetVariableName("SM");
	outputSlots_[0]->SetDescription("Pointer to static Model");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VOIDPTR); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Scene_AddStaticModel::PreLocalSolve()
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedItems.Size(); i++)
		{
			Component* sm = scene->GetComponent(trackedItems[i]);
			if (sm != NULL)
			{
				sm->Remove();
			}
		}
	}

	trackedItems.Clear();
}

void Scene_AddStaticModel::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{

	Scene* scene = GetSubsystem<IoGraph>()->scene;
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered.");
		return;
	}

	int id = inSolveInstance[0].GetInt();
	Node* node = scene->GetNode(id);

	if (!node)
	{
		URHO3D_LOGERROR("Could not find node: " + String(id));
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
		return;
	}

	StaticModel* sm = node->CreateComponent<StaticModel>();
	sm->SetModel(model->Clone());
	sm->SetMaterial(mat->Clone());

	trackedItems.Push(sm->GetID());

	outSolveInstance[0] = sm;
}
