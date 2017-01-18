#include "Sets_AddKeyValue.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Sets_AddKeyValue::iconTexture = "Textures/Icons/Sets_AddKeyValue.png";

Sets_AddKeyValue::Sets_AddKeyValue(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("AddKeyValue");
	SetFullName("Add Key Valye");
	SetDescription("Adds a key value pair to a variant map");

	AddInputSlot(
		"Map",
		"M",
		"Variant map to operate on. Can be empty.",
		VAR_VARIANTMAP,
		ITEM
	);

	AddInputSlot(
		"Key",
		"K",
		"Key of value to add",
		VAR_STRING,
		ITEM
	);

	AddInputSlot(
		"Value",
		"V",
		"Value add. This input is list access.",
		VAR_VARIANTVECTOR,
		LIST
	);

	AddOutputSlot(
		"Map out",
		"VM",
		"Variant map out.",
		VAR_VARIANTMAP,
		ITEM
	);
}

void Sets_AddKeyValue::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	VariantMap mapIn;
	if (inSolveInstance[0].GetType() == VAR_VARIANTMAP)
	{
		mapIn = inSolveInstance[0].GetVariantMap();
	}

	String key = inSolveInstance[1].GetString();
	VariantVector values = inSolveInstance[2].GetVariantVector();

	if (!values.Empty() && !key.Empty())
	{
		mapIn[key] = values;
	}

	outSolveInstance[0] = mapIn;
}
