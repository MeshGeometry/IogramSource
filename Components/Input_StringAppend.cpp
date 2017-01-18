#include "Input_StringAppend.h"

using namespace Urho3D;

String Input_StringAppend::iconTexture = "Textures/Icons/Input_StringAppend.png";

Input_StringAppend::Input_StringAppend(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("StringAppend");
	SetFullName("StringAppend");
	SetDescription("Appends a string.");


	AddInputSlot(
		"InputString",
		"A",
		"Input String",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"AppendString",
		"B",
		"String to append",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"String",
		"R",
		"Resulting string",
		VAR_STRING,
		DataAccess::ITEM
	);
}

void Input_StringAppend::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	if (inSolveInstance[0].GetType() == VAR_NONE || inSolveInstance[1].GetType() == VAR_NONE)
	{
		outSolveInstance[0] = Variant();
		return;
	}
	
	String inputString = inSolveInstance[0].GetString();
	String appendString = inSolveInstance[1].ToString();

	inputString.Append(appendString);

	outSolveInstance[0] = inputString;
}