#include "Input_StringReplace.h"

using namespace Urho3D;

String Input_StringReplace::iconTexture = "Textures/Icons/Input_StringReplace.png";

Input_StringReplace::Input_StringReplace(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("StringFormat");
	SetFullName("StringFormat");
	SetDescription("Appends or Creates a string with Formatting.");

	AddInputSlot(
		"String",
		"S",
		"String to work on",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"ReplaceThis",
		"A",
		"String to replace",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"ReplaceWith",
		"B",
		"Replacement string",
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

void Input_StringReplace::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{

	String inputString = inSolveInstance[0].GetString();
	String replaceThis = inSolveInstance[1].GetString();
	Variant replaceWith = inSolveInstance[2];

	inputString.Replace(replaceThis, replaceWith.ToString());

	outSolveInstance[0] = inputString;
}