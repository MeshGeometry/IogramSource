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


#include "Input_StringAppend.h"

using namespace Urho3D;

String Input_StringAppend::iconTexture = "Textures/Icons/Input_StringAppend.png";

Input_StringAppend::Input_StringAppend(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("StringAppend");
	SetFullName("StringAppend");
	SetDescription("Appends a string.");


	AddInputSlot(
		"InputString",
		"A",
		"Input String",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"AppendString",
		"B",
		"String to append",
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

void Input_StringAppend::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	if (inSolveInstance[0].GetType() == VAR_NONE || inSolveInstance[1].GetType() == VAR_NONE)
	{
		outSolveInstance[0] = Variant();
		return;
	}
	
	String inputString = inSolveInstance[0].GetString();
	String appendString = inSolveInstance[1].ToString();

	inputString.Append(appendString);

	outSolveInstance[0] = inputString;
}