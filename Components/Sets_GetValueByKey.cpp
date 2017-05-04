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
