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
