#include "Scene_SetGlobalVar.h"

#include "Scene_SetGlobalVar.h"
#include <Urho3D/Scene/SceneEvents.h>

#include <Urho3D/Core/Context.h>

#include <assert.h>

using namespace Urho3D;

String Scene_SetGlobalVar::iconTexture = "Textures/Icons/Scene_SetGlobalVar.png";

Scene_SetGlobalVar::Scene_SetGlobalVar(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("SetGlobalVar");
	SetFullName("Set Global Variant");
	SetDescription("Sets a global variant by name (key)");

	AddInputSlot(
		"Key",
		"K",
		"Key of variant to set.",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Variant",
		"V",
		"Variant to set",
		VAR_NONE,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Reset",
		"R",
		"Reset to null value",
		VAR_BOOL,
		DataAccess::ITEM
	);
	inputSlots_[2]->SetDefaultValue(false);
	inputSlots_[2]->DefaultSet();

	AddOutputSlot(
		"Variant",
		"V",
		"Set variant",
		VAR_NONE,
		DataAccess::ITEM
	);


}

void Scene_SetGlobalVar::PreLocalSolve()
{
	for (int i = 0; i < trackedItems.Size(); i++)
	{
		GetContext()->SetGlobalVar(trackedItems[i], Variant::EMPTY);
	}

	trackedItems.Clear();
}

void Scene_SetGlobalVar::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	String key = inSolveInstance[0].GetString();
	Variant var = inSolveInstance[1];
	bool reset = inSolveInstance[2].GetBool();

	if (reset) {
		var = Variant();
	}

	GetContext()->SetGlobalVar(key, var);
	trackedItems.Push(key);

	outSolveInstance[0] = var;
}

