#include "Scene_GetComponent.h"

using namespace Urho3D;

Urho3D::String Scene_GetComponent::iconTexture= "Textures/Icons/Scene_GetComponent.png";

Scene_GetComponent::Scene_GetComponent(Urho3D::Context* context):IoComponentBase(context, 0, 0)
{
	//set up component info
	SetName("GetComponent");
	SetFullName("Get Component");
	SetDescription("Gets a reference to a component from Node ID");

	//set up the slots
	AddInputSlot(
		"Node ID",
		"ID",
		"Node ID",
		VAR_INT,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Type",
		"T",
		"Type of component to return",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Recursive",
		"R",
		"Search node recursively",
		VAR_BOOL,
		DataAccess::ITEM
	);
	inputSlots_[2]->SetDefaultValue(true);

	AddInputSlot(
		"",
		"",
		"",
		VAR_INT,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Reference",
		"Ptr",
		"Reference to component",
		VAR_PTR,
		DataAccess::ITEM
	);
}

void Scene_GetComponent::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	
	String typeName = inSolveInstance[1].GetString();

	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	if (!scene)
	{
		URHO3D_LOGERROR("No scene detected.");
		outSolveInstance[0] = Variant();
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
		URHO3D_LOGERROR("No node found");
		outSolveInstance[0] = Variant();
		return;
	}

	bool recursive = inSolveInstance[2].GetBool();
	Component* comp = node->GetComponent(StringHash(typeName), recursive);

	if (!comp)
	{
		outSolveInstance[0] = Variant();
		return;
	}

	outSolveInstance[0] = comp;
}

