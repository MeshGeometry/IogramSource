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


#include "Vector_ColorPalette.h"
#include "ColorDefs.h"
#include <assert.h>

#include <Urho3D/Math/Vector3.h>

#include <Eigen/Core>

using namespace Urho3D;

String Vector_ColorPalette::iconTexture = "Textures/Icons/Vector_ColorPalette.png";

Vector_ColorPalette::Vector_ColorPalette(Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("ClosePoint");
	SetFullName("Closest Point");
	SetDescription("Which point in list is closest");
	SetGroup(IoComponentGroup::VECTOR);
	SetSubgroup("Point");

	inputSlots_[0]->SetName("StartColor");
	inputSlots_[0]->SetVariableName("C");
	inputSlots_[0]->SetDescription("Start Color");
	inputSlots_[0]->SetVariantType(VariantType::VAR_COLOR);
	inputSlots_[0]->SetDefaultValue(BRIGHT_ORANGE);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Length");
	inputSlots_[1]->SetVariableName("N");
	inputSlots_[1]->SetDescription("Number of colors to generate");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(10);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Variance");
	inputSlots_[2]->SetVariableName("V");
	inputSlots_[2]->SetDescription("Variance of colors");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(0.1f);
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("ResultingColors");
	outputSlots_[0]->SetVariableName("R");
	outputSlots_[0]->SetDescription("Resulting Colors");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTVECTOR);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

}

void Vector_ColorPalette::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Color startCol = inSolveInstance[0].GetColor();
	int numCols = inSolveInstance[1].GetInt();
	float variance = inSolveInstance[2].GetFloat();

	VariantVector colsOut;
	Urho3D::SetRandomSeed((unsigned)startCol.SumRGB());
	for (int i = 0; i < numCols; i++)
	{
		float r = Urho3D::Random(startCol.r_ - variance, startCol.r_ + variance);
		float g = Urho3D::Random(startCol.g_ - variance, startCol.g_ + variance);
		float b = Urho3D::Random(startCol.b_ - variance, startCol.b_ + variance);
		float a = Urho3D::Random(startCol.a_ - variance, startCol.a_ + variance);

		colsOut.Push(Color(r, g, b, a));
	}

	outSolveInstance[0] = colsOut;
}