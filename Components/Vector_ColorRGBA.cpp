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


#include "Vector_ColorRGBA.h"

#include "Urho3D/Graphics/Technique.h"

using namespace Urho3D;

String Vector_ColorRGBA::iconTexture = "Textures/Icons/Vector_ColorRGBA.png";

Vector_ColorRGBA::Vector_ColorRGBA(Urho3D::Context* context) : IoComponentBase(context, 4, 1)
{
	SetName("Color");
	SetFullName("Construct Color");
	SetDescription("Construct a color from RGBA values");
	SetGroup(IoComponentGroup::VECTOR);
	SetSubgroup("Vector");

	inputSlots_[0]->SetName("R");
	inputSlots_[0]->SetVariableName("R");
	inputSlots_[0]->SetDescription("Red");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(0.1f);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("G");
	inputSlots_[1]->SetVariableName("G");
	inputSlots_[1]->SetDescription("Green");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(0.1f);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("B");
	inputSlots_[2]->SetVariableName("B");
	inputSlots_[2]->SetDescription("Blue");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(0.1f);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("A");
	inputSlots_[3]->SetVariableName("A");
	inputSlots_[3]->SetDescription("Alpha");
	inputSlots_[3]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(1.0f);
	inputSlots_[3]->DefaultSet();

	outputSlots_[0]->SetName("Color");
	outputSlots_[0]->SetVariableName("C");
	outputSlots_[0]->SetDescription("Color");
	outputSlots_[0]->SetVariantType(VariantType::VAR_COLOR);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}


void Vector_ColorRGBA::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{

	float r = Clamp(inSolveInstance[0].GetFloat(), 0.0f, 1.0f);
	float g = Clamp(inSolveInstance[1].GetFloat(), 0.0f, 1.0f);
	float b = Clamp(inSolveInstance[2].GetFloat(), 0.0f, 1.0f);
	float a = Clamp(inSolveInstance[3].GetFloat(), 0.0f, 1.0f);

	outSolveInstance[0] = Color(r, g, b, a);
}