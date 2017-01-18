#include "Scene_CloneNode.h"

#include "IoGraph.h"

#include "Geomlib_ConstructTransform.h"
#include <Urho3D/Graphics/StaticModel.h>

using namespace Urho3D;

String Scene_CloneNode::iconTexture = "Textures/Icons/Scene_CloneNode.png";

Scene_CloneNode::Scene_CloneNode(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("Clone Node");
	SetFullName("Clones a node and all of its components.");
	SetDescription("Clones a node");

	AddInputSlot(
		"NodeID",
		"ID",
		"ID of node to clone",
		VAR_INT,
		ITEM
	);

	AddInputSlot(
		"Transform",
		"T",
		"Transform of cloned node",
		VAR_MATRIX3X4,
		ITEM
	);

	AddInputSlot(
		"ParentID",
		"P",
		"Optional parent of cloned node",
		VAR_INT,
		ITEM
	);

	AddOutputSlot(
		"NodeID",
		"ID",
		"ID of new node",
		VAR_INT,
		ITEM
	);
}

Scene_CloneNode::~Scene_CloneNode()
{
	PreLocalSolve();
}

void Scene_CloneNode::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	int oldNodeID = inSolveInstance[0].GetInt();
	if (scene == NULL || oldNodeID == 0)
	{
		URHO3D_LOGERROR("Null scene encountered.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}


	//add new n
	Node* oldNode = scene->GetNode(oldNodeID);
	if (!oldNode)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}


	//clone
	Node* clonedNode = oldNode->Clone();
	clonedNode->SetName(oldNode->GetName() + "_Clone");

	//set parent if applicable
	int parentID = inSolveInstance[2].GetInt();
	Node* parentNode = scene->GetNode(parentID);
	if (parentNode)
	{
		clonedNode->SetParent(parentNode);
	}
	else
	{
		clonedNode->SetParent(oldNode->GetParent());
	}
	

	if (inSolveInstance[1].GetType() != VAR_NONE)
	{
		// construct transform:
		Matrix3x4 xform = Geomlib::ConstructTransform(inSolveInstance[1], clonedNode->GetTransform());

		//apply transform	
		clonedNode->SetPosition(xform.Translation());
		clonedNode->SetRotation(xform.Rotation());
		clonedNode->SetScale(xform.Scale());
	}
	else
	{
		//probably nothing to do here...
	}

	//URHO3D_LOGINFO("Scene object count: " + String(scene->GetNumChildren()));
	//PODVector<StaticModel*> sms;
	//scene->GetComponents<StaticModel>(sms, true);
	//URHO3D_LOGINFO("Scene component count: " + String(sms.Size()));
	//track
	trackedNodes.Push(clonedNode->GetID());

	outSolveInstance[0] = clonedNode->GetID();
}

void Scene_CloneNode::PreLocalSolve()
{
	//delete old nodes
	Scene* scene = GetSubsystem<IoGraph>()->scene;
	for (int i = 0; i < trackedNodes.Size(); i++)
	{
		Node* oldNode = scene->GetNode(trackedNodes[i]);
		if (oldNode)
		{
			oldNode->Remove();
		}

	}

	trackedNodes.Clear();
}


