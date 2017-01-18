#include "Sets_GetValueByKey.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Sets_GetValueByKey::iconTexture = "Textures/Icons/Sets_GetValueByKey.png";

Sets_GetValueByKey::Sets_GetValueByKey(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("GetValueByKey");
	SetFullName("Get Value by Key");
	SetDescription("Gets a value by key name.");

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

	AddOutputSlot(
		"Value out",
		"V",
		"Value out",
		VAR_VARIANTVECTOR,
		LIST
	);
}

void Sets_GetValueByKey::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	
	if (inSolveInstance[0].GetType() != VAR_VARIANTMAP)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	VariantMap mapIn = inSolveInstance[0].GetVariantMap();

	String key = inSolveInstance[1].GetString();
	
	if (!mapIn.Keys().Contains(key))
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Variant value = mapIn[key];
	
	if (!value.GetType() == VAR_VARIANTVECTOR)
	{
		VariantVector valOut;
		valOut.Push(value);
		outSolveInstance[0] = valOut;
	}
	else
	{
		outSolveInstance[0] = value;
	}

	
}
