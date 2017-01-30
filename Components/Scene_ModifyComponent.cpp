#include "Scene_ModifyComponent.h"

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

String Scene_ModifyComponent::iconTexture = "Textures/Icons/Scene_ModifyComponent.png";

Scene_ModifyComponent::Scene_ModifyComponent(Context* context) : IoComponentBase(context, 0, 0)
{
	//set up component info
	SetName("ModifyComponent");
	SetFullName("Modify Component");
	SetDescription("Modifies the properties of a native component.");

	//set up the slots
	AddInputSlot(
		"Component Reference",
		"C",
		"Pointer reference to native component.",
		VAR_PTR,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Property",
		"P",
		"Name of Property to modify.",
		VAR_STRING,
		DataAccess::LIST
	);

	AddInputSlot(
		"Value",
		"V",
		"Value with which to modify property.",
		VAR_FLOAT,
		DataAccess::LIST
	);

	AddOutputSlot(
		"Reference",
		"Ptr",
		"Reference to modified component",
		VAR_PTR,
		DataAccess::ITEM
	);
}

//work function
void Scene_ModifyComponent::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	Component* comp = (Component*)inSolveInstance[0].GetPtr();
	if (comp == NULL)
	{
		
		comp = (Component*)inSolveInstance[0].GetVoidPtr();
		if (!comp)
		{
			URHO3D_LOGERROR("Could not cast input as a native Component!");
			outSolveInstance[0] = Variant();
			return;
		}

	}

	//iterate over properties and values and try to assign
	VariantVector propNames = inSolveInstance[1].GetVariantVector();
	VariantVector propVals = inSolveInstance[2].GetVariantVector();

	int count = Min(propNames.Size(), propVals.Size());

	for (int i = 0; i < count; i++)
	{
		//make sure that the supplied property actually exists
		String attributeName = propNames[i].GetString();
		Variant attriVar = comp->GetAttribute(attributeName);

		if (attriVar.GetType() != VAR_NONE)
		{
			if (attriVar.GetType() == propVals[i].GetType())
			{
				comp->SetAttribute(attributeName, propVals[i]);
			}
			else
			{
				URHO3D_LOGERROR("Mismatch in supplied attribuate type");
			}
		}
		else
		{
			URHO3D_LOGERROR("Could not find attribute: " + attributeName + " in component: " + comp->GetTypeNameStatic());
		}
	}

	outSolveInstance[0] = comp;
}