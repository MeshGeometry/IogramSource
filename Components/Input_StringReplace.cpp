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