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


#include "Input_ColorWheel.h"

using namespace Urho3D;

String Input_ColorWheel::iconTexture = "Textures/Icons/Input_ColorWheel.png";

Input_ColorWheel::Input_ColorWheel(Urho3D::Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("ColorWheel");
	SetFullName("Color Wheel");
	SetDescription("...");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("INPUT");

	inputSlots_[0]->SetName("Intensity");
	inputSlots_[0]->SetVariableName("");
	inputSlots_[0]->SetDescription("Intensity of Bloom");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(75.0f);
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("Color");
	outputSlots_[0]->SetVariableName("C");
	outputSlots_[0]->SetDescription("Color");
	outputSlots_[0]->SetVariantType(VariantType::VAR_COLOR);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Input_ColorWheel::PreLocalSolve()
{

}

void Input_ColorWheel::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	outSolveInstance[0] = Color::RED;
}