#include "Scene_GetNode.h"

#include "Scene_GetNode.h"
#include <Urho3D/Scene/SceneEvents.h>

#include <assert.h>

#include <Urho3D/Core/Context.h>

using namespace Urho3D;

String Scene_GetNode::iconTexture = "Textures/Icons/Scene_GetNode.png";

Scene_GetNode::Scene_GetNode(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("GetNode");
	SetFullName("Get Node");
	SetDescription("Finds a Node");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("Primitive");

	AddInputSlot(
		"NodeID",
		"ID",
		"ID of node to inspect.",
		VAR_INT,
		DataAccess::ITEM
	);

	AddInputSlot();

	AddOutputSlot(
		"NodeID",
		"ID",
		"Node ID",
		VAR_INT,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Name",
		"N",
		"Name",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Transform",
		"T",
		"Transform",
		VAR_MATRIX3X4,
		DataAccess::ITEM
	);
}

void Scene_GetNode::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	if (!scene)
	{
		URHO3D_LOGERROR("No scene detected.");
		outSolveInstance[0] = Variant();
		outSolveInstance[1] = Variant();
		return;
	}

	Node* node = NULL;

	if (inSolveInstance[0].GetType() == VAR_INT)
	{
		node = scene->GetNode(inSolveInstance[0].GetInt());
	}

	else if (inSolveInstance[0].GetType() == VAR_STRING)
	{
		String name = inSolveInstance[0].GetString();
		node = scene->GetChild(name, true);
	}

	if (!node)
	{
		URHO3D_LOGERROR("No node with that id found.");
		outSolveInstance[0] = Variant();
		outSolveInstance[1] = Variant();
		return;
	}

	outSolveInstance[0] = node->GetID();
	outSolveInstance[1] = node->GetName();
	Matrix3x4 xform = node->GetWorldTransform();
	Vector3 wPos = node->GetPosition();
	Vector3 trans = xform.Translation();
	outSolveInstance[2] = xform;
}