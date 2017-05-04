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


#include "Maths_UnitizeVector.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_UnitizeVector::iconTexture = "Textures/Icons/Maths_UnitizeVector.png";

Maths_UnitizeVector::Maths_UnitizeVector(Urho3D::Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("UnitizeVector");
	SetFullName("UnitizeVector");
	SetDescription("Computes a unit vector from a vector");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	Variant default_vec = Vector3(0.0f, 0.0f, 0.0f);

	inputSlots_[0]->SetName("Vector");
	inputSlots_[0]->SetVariableName("X");
	inputSlots_[0]->SetDescription("Vector");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDefaultValue(default_vec);
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("UnitVector");
	outputSlots_[0]->SetVariableName("U");
	outputSlots_[0]->SetDescription("Unit Vector in direction of X");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

}

void Maths_UnitizeVector::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////
	Vector3 X = inSolveInstance[0].GetVector3();
	Vector3 default_vec = Vector3(0.0f, 0.0f, 0.0f);
	Variant unitVec;

	if (X == default_vec) {
		unitVec = default_vec;
	}
	else
		unitVec = X.Normalized();

	outSolveInstance[0] = unitVec;

	////////////////////////////////////////////////////////////
}
