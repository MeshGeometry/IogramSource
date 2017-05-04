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


#include "Vector_DeconstructVector.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>

using namespace Urho3D;

String Vector_DeconstructVector::iconTexture = "Textures/Icons/Vector_DeconstructVector.png";

Vector_DeconstructVector::Vector_DeconstructVector(Context* context) :
	IoComponentBase(context, 1, 3)
{
	SetName("DecVec");
	SetFullName("Deconstruct Vector");
	SetDescription("Deconstruct a vector into its components");
	SetGroup(IoComponentGroup::VECTOR);
	SetSubgroup("Vector");

	inputSlots_[0]->SetName("Vector");
	inputSlots_[0]->SetVariableName("V");
	inputSlots_[0]->SetDescription("Vector to deconstruct");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Vector3(0.0f, 0.0f, 0.0f));
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("X-Coordinate");
	outputSlots_[0]->SetVariableName("X");
	outputSlots_[0]->SetDescription("X-Coordinate of vector");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Y-Coordinate");
	outputSlots_[1]->SetVariableName("Y");
	outputSlots_[1]->SetDescription("Y-Coordinate of vector");
	outputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[2]->SetName("Z-Coordinate");
	outputSlots_[2]->SetVariableName("Z");
	outputSlots_[2]->SetDescription("Z-Coordinate of vector");
	outputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[2]->SetDataAccess(DataAccess::ITEM);
}

void Vector_DeconstructVector::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	Variant xVariant, yVariant, zVariant;

	///////////////////
	// EXTRACT & VERIFY

	if (inSolveInstance[0].GetType() != VariantType::VAR_VECTOR3) {
		URHO3D_LOGWARNING("V must be a valid Vector3");
		outSolveInstance[0] = Variant();
		outSolveInstance[1] = Variant();
		outSolveInstance[2] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Variant inVariant = inSolveInstance[0];
	Vector3 vec = inVariant.GetVector3();
	xVariant = Variant(vec.x_);
	yVariant = Variant(vec.y_);
	zVariant = Variant(vec.z_);

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = xVariant;
	outSolveInstance[1] = yVariant;
	outSolveInstance[2] = zVariant;
}