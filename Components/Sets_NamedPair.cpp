
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


#include "Sets_NamedPair.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Sets_NamedPair::iconTexture = "Textures/Icons/Sets_NamedPair.png";

Sets_NamedPair::Sets_NamedPair(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("AddKeyValue");
	SetFullName("Add Key Valye");
	SetDescription("Adds a key value pair to a variant map");

	AddInputSlot(
		"Name",
		"N",
		"Name of to bind to variant",
		VAR_STRING,
		ITEM
	);

	AddInputSlot(
		"Value",
		"V",
		"Value of the variant",
		VAR_STRING,
		ITEM
	);

	AddOutputSlot(
		"Pair",
		"P",
		"Variant map out.",
		VAR_VARIANTMAP,
		ITEM
	);
}

void Sets_NamedPair::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	String name = inSolveInstance[0].GetString();
	if (name.Empty() || inSolveInstance[1].GetType() == VAR_NONE)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}


	VariantMap pairOut;
	pairOut["name"] = name;
	pairOut["value"] = inSolveInstance[1];


	outSolveInstance[0] = pairOut;
}
