#include "Scene_GetGlobalVar.h"

#include "Scene_GetGlobalVar.h"
#include <Urho3D/Scene/SceneEvents.h>

#include <assert.h>

using namespace Urho3D;

String Scene_GetGlobalVar::iconTexture = "Textures/Icons/Scene_GetGlobalVar.png";

Scene_GetGlobalVar::Scene_GetGlobalVar(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("GetGlobalVar");
	SetFullName("Get Global Variant");
	SetDescription("Gets a global variant by name (key)");

	AddInputSlot(
		"VarName",
		"K",
		"Key of variant to get.",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"DefaultValue",
		"D",
		"Default value if get fails.",
		VAR_FLOAT,
		DataAccess::ITEM
	);
	inputSlots_[1]->SetDefaultValue(0.0f);
	inputSlots_[1]->DefaultSet();

	AddOutputSlot(
		"Variant",
		"V",
		"Returned variant",
		VAR_NONE,
		DataAccess::ITEM
	);


}

void Scene_GetGlobalVar::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	String key = inSolveInstance[0].GetString();
	Variant v = GetContext()->GetGlobalVar(key);

	if (v.GetType() == VAR_NONE)
	{
		v = inSolveInstance[1];
	}

	outSolveInstance[0] = v;
}