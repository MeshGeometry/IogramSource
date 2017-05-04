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


#include "Sets_DivideDomain3D.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Sets_DivideDomain3D::iconTexture = "Textures/Icons/Sets_DivideDomain3D.png";

Sets_DivideDomain3D::Sets_DivideDomain3D(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("DivideDomain");
	SetFullName("");
	SetDescription("Divides a 3D domain into subdomains");
	SetGroup(IoComponentGroup::SETS);
	SetSubgroup("Sequence");

	inputSlots_[0]->SetName("MinVec");
	inputSlots_[0]->SetVariableName("A");
	inputSlots_[0]->SetDescription("Range minimum (Vector3)");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDefaultValue(Variant(Vector3::ZERO));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("MaxVec");
	inputSlots_[1]->SetVariableName("B");
	inputSlots_[1]->SetDescription("Range maximum (Vector3)");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDefaultValue(Variant(Vector3::ONE));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("DivisionPattern");
	inputSlots_[2]->SetVariableName("N");
	inputSlots_[2]->SetDescription("Number of partitions (Vector3)");
	inputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[2]->SetDefaultValue(Variant(Vector3(5,5,3)));
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Subdomains");
	outputSlots_[0]->SetVariableName("S");
	outputSlots_[0]->SetDescription("Divided Domain");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);
}

void Sets_DivideDomain3D::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////
	// Verify input slot 0
	VariantType type0 = inSolveInstance[0].GetType();
	if (!(type0 == VariantType::VAR_VECTOR3 )) {
		URHO3D_LOGWARNING("Min must be a valid Vector3.");
		outSolveInstance[0] = Variant();
		return;
	}
	Vector3 min = inSolveInstance[0].GetVector3();

	// Verify input slot 1
	VariantType type1 = inSolveInstance[1].GetType();
	if (!(type1 == VariantType::VAR_VECTOR3 )) {
		URHO3D_LOGWARNING("Max must be a valid Vector3.");
		outSolveInstance[0] = Variant();
		return;
	}
	Vector3 max = inSolveInstance[1].GetVector3();

	// Verify input slot 2
	VariantType type2 = inSolveInstance[2].GetType();
	if (!(type2 == VariantType::VAR_VECTOR3)) {
		URHO3D_LOGWARNING("Max must be a valid Vector3.");
		outSolveInstance[0] = Variant();
		return;
	}
	Vector3 N = inSolveInstance[2].GetVector3();
	if (N.x_ == 0 || N.y_ == 0) {
		URHO3D_LOGWARNING("x and y coordinates of N must not be zero");
		outSolveInstance[0] = Variant();
		return;
	}

	bool planar = false;
	if (N.z_ == 0)
		planar = true;

	VariantVector seriesList;
//	seriesList.Push(min);
	float chunk_x = (max.x_ - min.x_) / N.x_;
	float chunk_y = (max.y_ - min.y_) / N.y_;
	float chunk_z = 0;
	if (!planar)
		chunk_z = (max.z_ - min.z_) / N.z_;

	for (int i = 0; i < N.x_ + 1; ++i) {
		for (int j = 0; j < N.y_ + 1; ++j) {
			for (int k = 0; k < N.z_ + 1; ++k) {
				float X = min.x_ + i*chunk_x;
				float Y = min.y_ + j*chunk_y;
				float Z = min.z_ + k*chunk_z;

				// if it is planar, map to the XZ plane:
				if (planar)
				{
					Z = Y;
					Y = 0.0f;
				}

				seriesList.Push(Vector3(X, Y, Z));
			}
		}
	}
	Variant out(seriesList);
	outSolveInstance[0] = out;

	////////////////////////////////////////////////////////////
}
