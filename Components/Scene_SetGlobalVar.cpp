//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


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

