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


#include "Input_Float.h"

#include "IoGraph.h"

#include <Urho3D/UI/UIEvents.h>

#include <assert.h>

using namespace Urho3D;

String Input_Float::iconTexture = "Textures/Icons/Input_Float.png";

Input_Float::Input_Float(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("Float");
	SetFullName("Float");
	SetDescription("Creates or casts a float");


	AddInputSlot(
		"Number",
		"X",
		"Number to cast to a float",
		VAR_FLOAT,
		DataAccess::ITEM
		);

	AddOutputSlot(
		"Float",
		"F",
		"Float",
		VAR_FLOAT,
		DataAccess::ITEM
		);

}



void Input_Float::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{

    Variant in_var = inSolveInstance[0];
    float out_float = 0.0f;
    
    if (in_var.GetType() == VAR_STRING) {
        String in_string = in_var.GetString();
        out_float = Urho3D::ToFloat(in_string);
    }
    
    else
        out_float = in_var.GetFloat();
    
    
    outSolveInstance[0] = Variant(out_float);
}
