#include "Maths_EvalFunction.h"

#include "Urho3D/AngelScript/Addons.h"
#include "Urho3D/AngelScript/APITemplates.h"
#include "Urho3D/AngelScript/Script.h";

using namespace Urho3D;

String Maths_EvalFunction::iconTexture = "Textures/Icons/Maths_EvalFunction.png";

Maths_EvalFunction::Maths_EvalFunction(Urho3D::Context* context) : IoComponentBase(context, 4, 1)
{
	//set up inputs
	SetName("EvalFunction");
	SetFullName("Evaluate Function");
	SetDescription("Evaluates a basic math function.");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("First Arg");
	inputSlots_[0]->SetVariableName("X");
	inputSlots_[0]->SetDescription("First input");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDefaultValue(1.0f);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Second Arg");
	inputSlots_[1]->SetVariableName("Y");
	inputSlots_[1]->SetDescription("Second input");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDefaultValue(1.0f);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Third Arg");
	inputSlots_[2]->SetVariableName("Z");
	inputSlots_[2]->SetDescription("Third input");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDefaultValue(1.0f);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("Function");
	inputSlots_[3]->SetVariableName("FN");
	inputSlots_[3]->SetDescription("Function definition in Script");
	inputSlots_[3]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[3]->SetDefaultValue("Pow(X, Y) + Z");
	inputSlots_[3]->DefaultSet();

	//set up outputs
	outputSlots_[0]->SetName("Result");
	outputSlots_[0]->SetVariableName("R");
	outputSlots_[0]->SetDescription("Evaluated result");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

}

void Maths_EvalFunction::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	//get the definition of the function
	String function = inSolveInstance[3].GetString();

	//get the arguments
	float x = inSolveInstance[0].GetFloat();
	float y = inSolveInstance[1].GetFloat();
	float z = inSolveInstance[2].GetFloat();

	//replace the argument place holders with the function variables
	function.Replace("X", String(x), false);
	function.Replace("Y", String(y), false);
	function.Replace("Z", String(z), false);

	//run the function as a script function
	Script* script_system = GetSubsystem<Script>();
	String line = "float result =" + function + "; SetGlobalVar(\"" + ID + "\", result);";
	bool res = script_system->Execute(line);

	float result = script_system->GetGlobalVar(ID).GetFloat();
	//export the result
	outSolveInstance[0] = result;
}