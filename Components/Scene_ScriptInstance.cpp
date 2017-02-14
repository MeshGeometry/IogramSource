#include "Scene_ScriptInstance.h"

#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "Urho3D/UI/Button.h"
#include "Urho3D/UI/Text.h"
#include "Urho3D/ThirdParty/AngelScript/angelscript.h"

#include "Urho3D/AngelScript/APITemplates.h"
#include "Urho3D/AngelScript/Script.h"
#include "Urho3D/AngelScript/ScriptFile.h"

using namespace Urho3D;

String Scene_ScriptInstance::iconTexture = "Textures/Icons/Scene_ScriptInstance.png";

Scene_ScriptInstance::Scene_ScriptInstance(Urho3D::Context* context) : IoComponentBase(context, 4, 3)
{
	SetName("ScriptInstance");
	SetFullName("Script Instance");
	SetDescription("Executes a script from file");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("");

	inputSlots_[0]->SetName("ScriptFile");
	inputSlots_[0]->SetVariableName("SC");
	inputSlots_[0]->SetDescription("Path to script file");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("X");
	inputSlots_[1]->SetVariableName("X");
	inputSlots_[1]->SetDescription("Input variable called X");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Y");
	inputSlots_[2]->SetVariableName("Y");
	inputSlots_[2]->SetDescription("Input variable called Y");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("Z");
	inputSlots_[3]->SetVariableName("Z");
	inputSlots_[3]->SetDescription("Input variable called Z");
	inputSlots_[3]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->DefaultSet();

	outputSlots_[0]->SetName("A");
	outputSlots_[0]->SetVariableName("A");
	outputSlots_[0]->SetDescription("Output variable called A");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("B");
	outputSlots_[1]->SetVariableName("B");
	outputSlots_[1]->SetDescription("Output variable called B");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[2]->SetName("C");
	outputSlots_[2]->SetVariableName("C");
	outputSlots_[2]->SetDescription("Output variable called C");
	outputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[2]->SetDataAccess(DataAccess::ITEM);

}


void Scene_ScriptInstance::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	String path = inSolveInstance[0].GetString();
	
	//run the script if one is present
	Script* script_engine = GetContext()->GetSubsystem<Script>();

	ResourceCache* cache = GetContext()->GetSubsystem<ResourceCache>();
	FileSystem* fs = GetContext()->GetSubsystem<FileSystem>();
	ScriptFile* script_file = cache->GetResource<ScriptFile>(path);
	cache->ReloadResource(script_file);

	bool exists = cache->Exists(path);

	if (script_file)
	{
		String declaration = "void SolveInstance(const VariantMap& inputs, VariantMap& outputs)";
		bool res = false;
		if (script_file->GetFunction(declaration))
		{
			//create a map between variable names and input values
			VariantMap inArgs;
			for (int i = 0; i < inSolveInstance.Size(); i++)
			{
				inArgs[inputSlots_[i]->GetVariableName()] = inSolveInstance[i];
			}

			//create a map between variable names and output values
			VariantMap outArgs;
			for (int i = 0; i < outSolveInstance.Size(); i++)
			{
				outArgs[outputSlots_[i]->GetVariableName()] = 0.0f;
			}

			//push these to a vector for passing to the script
			VariantVector vars;
			vars.Push(inArgs);
			vars.Push(outArgs);

			//finally, call the script
			res = script_file->Execute(declaration, vars);

			//recover the output map and push to outSolveInstance
			VariantMap outMap = vars[1].GetVariantMap();

			for (int i = 0; i < outSolveInstance.Size(); i++)
			{
				String keyName = outputSlots_[i]->GetVariableName();
				if (outMap.Contains(keyName))
				{
					outSolveInstance[i] = outMap[keyName];
				}
				else
				{
					outSolveInstance[i] = Variant(VAR_NONE);
				}
			}
		}
		else if (script_file->GetFunction("void Start()"))
		{
			res = script_file->Execute("void Start()");
		}
		else
		{
			URHO3D_LOGINFO("Could not start script file");
		}
	}
	else {
		URHO3D_LOGINFO("Did not find script_file");
	}

}