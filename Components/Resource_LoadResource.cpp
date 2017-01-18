#include "Resource_LoadResource.h"

#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Resource_LoadResource::iconTexture = "Textures/Icons/Resource_LoadResource.png";

Resource_LoadResource::Resource_LoadResource(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("LoadResource");
	SetFullName("Load Resource");
	SetDescription("Loads a resource from a path");
	SetGroup(IoComponentGroup::DISPLAY);
	SetSubgroup("");

	inputSlots_[0]->SetName("Path");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Path to Resource file (e.g. \"Models/TeaPot.mdl\")");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	//inputSlots_[0]->SetDefaultValue("Models/Box.mdl");

	inputSlots_[1]->SetName("Type");
	inputSlots_[1]->SetVariableName("T");
	inputSlots_[1]->SetDescription("Type Name. Must be a registered class type. (e.g. \"model\")");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	//inputSlots_[1]->SetDefaultValue("Model");

	outputSlots_[0]->SetName("ResourcePointer");
	outputSlots_[0]->SetVariableName("R");
	outputSlots_[0]->SetDescription("Void Pointer to Resource");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VOIDPTR); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Resource_LoadResource::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	String resPath = inSolveInstance[0].GetString();
	URHO3D_LOGINFO("model path: " + resPath);
	String typeName = inSolveInstance[1].GetString();
	URHO3D_LOGINFO("type name: " + typeName);
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	StringHash typeHash(typeName);
	if (GetContext()->GetTypeName(typeHash).Empty())
	{
		URHO3D_LOGERROR("Type name not registered with Context: " + typeName);
		return;
	}

	Resource* resource = cache->GetResource(typeHash, resPath);
	
	if (resource)
	{
		outSolveInstance[0] = resource;
	}
	else
	{
		URHO3D_LOGERROR("Could not load resource: " + resPath);
	}
}
