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