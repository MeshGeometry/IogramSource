#include "Scene_AddComponent.h"

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
	if (node)
	{
		Component* comp = node->CreateComponent(compTypeName);
		if (comp)
		{
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