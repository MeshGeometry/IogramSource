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


#include "Maths_VectorLength.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_VectorLength::iconTexture = "Textures/Icons/Maths_VectorLength.png";

Maths_VectorLength::Maths_VectorLength(Urho3D::Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("VectorLength");
	SetFullName("Vector Length");
	SetDescription("Computes the length of a vector");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	Variant default_vec = Vector3(0.0f, 0.0f, 0.0f);

	inputSlots_[0]->SetName("Vector");
	inputSlots_[0]->SetVariableName("X");
	inputSlots_[0]->SetDescription("Vector");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDefaultValue(default_vec);
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("Length");
	outputSlots_[0]->SetVariableName("L");
	outputSlots_[0]->SetDescription("Float representing length of X");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

}

void Maths_VectorLength::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////
	if (inSolveInstance[0].GetType() != VariantType::VAR_VECTOR3) {
		outSolveInstance[0] = Variant();
		return;
	}

	Vector3 X = inSolveInstance[0].GetVector3();

	Variant length;

	length = X.Length();

	outSolveInstance[0] = length;

	////////////////////////////////////////////////////////////
}
