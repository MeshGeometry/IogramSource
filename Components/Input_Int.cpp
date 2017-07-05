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


#include "Input_Int.h"

#include "IoGraph.h"

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Core/StringUtils.h>

#include <assert.h>

using namespace Urho3D;

String Input_Int::iconTexture = "Textures/Icons/Input_Integer.png";

Input_Int::Input_Int(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("Integer");
	SetFullName("Integer");
	SetDescription("Creates or casts an integer");


	AddInputSlot(
		"Number",
		"X",
		"Number to cast to an int",
		VAR_INT,
		DataAccess::ITEM
		);

	AddOutputSlot(
		"Integer",
		"I",
		"Integer",
		VAR_INT,
		DataAccess::ITEM
		);

}


void Input_Int::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{

	Variant in_var = inSolveInstance[0];
	int out_int = 0;

	if (in_var.GetType() == VAR_STRING) {
		String in_string = in_var.GetString();
		out_int = Urho3D::ToInt(in_string);
	}

	else
		out_int = in_var.GetInt();


	outSolveInstance[0] = Variant(out_int);

}
