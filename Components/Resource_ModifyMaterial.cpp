#include "Resource_ModifyMaterial.h"

#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "IoGraph.h"

using namespace Urho3D;

String Resource_ModifyMaterial::iconTexture = "";

Resource_ModifyMaterial::Resource_ModifyMaterial(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("ModifyMaterial");
	SetFullName("Modify Material");
	SetDescription("Edit a material with new parameters");
	SetGroup(IoComponentGroup::DISPLAY);
	SetSubgroup("");

	inputSlots_[0]->SetName("Technique");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Path to material");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue("Materials/DefaultGrey.xml");
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Parameters");
	inputSlots_[1]->SetVariableName("P");
	inputSlots_[1]->SetDescription("Key-Value pairs of parameters to modify");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[1]->SetDataAccess(DataAccess::LIST);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Textures");
	inputSlots_[2]->SetVariableName("T");
	inputSlots_[2]->SetDescription("Key-Value pairs of tex units and resource paths");
	inputSlots_[2]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[2]->SetDataAccess(DataAccess::LIST);
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Material");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Pointer to material");
	outputSlots_[0]->SetVariantType(VariantType::VAR_PTR);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}


void Resource_ModifyMaterial::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{

	ResourceCache* cache = GetSubsystem<ResourceCache>();

	String matPath = inSolveInstance[0].GetString();

	//clone the material?
	bool clone = inSolveInstance[2].GetBool();
	Material* mat = mat = cache->GetResource<Material>(matPath);

	//check that all is well
	if (!mat)
	{
		URHO3D_LOGERROR("Could not load material: " + matPath);
		return;
	}

	auto shaderParams = mat->GetShaderParameters();


	//iterate over key value pairs and try to adjust the parameter
	VariantVector keyPairList = inSolveInstance[1].GetVariantVector();

	//insert some test data
	keyPairList.Clear();
	VariantMap testPair;
	testPair["MatDiffColor"] = Urho3D::Random() * Color::RED;
	keyPairList.Push(testPair);

	VariantMap testPair2;
	testPair2["MatDiffColor"] = Urho3D::Random() * Color::BLUE;
	keyPairList.Push(testPair2);

	

	for (int i = 0; i < keyPairList.Size(); i++)
	{
		VariantMap keyPair = keyPairList[i].GetVariantMap();

		VariantMap::ConstIterator itr;
		for (itr = keyPair.Begin(); itr != keyPair.End(); itr++)
		{

			if (shaderParams.Contains(itr->first_))
			{
				MaterialShaderParameter param = shaderParams[itr->first_];
				mat->SetShaderParameter(param.name_, itr->second_);
			}

		}
	}

	//output
	outSolveInstance[0] = matPath;

}

