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


#include "Geometry_LookAt.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Geometry_LookAt::iconTexture = "Textures/Icons/Geometry_LookAt.png";


Geometry_LookAt::Geometry_LookAt(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("ConstructRotation");
	SetFullName("Construct Rotation Quaternion");
	SetDescription("Construct a rotation quaternion from axis and angle");

	inputSlots_[0]->SetName("TargetPoint");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Target Point");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDefaultValue(Vector3(0, 0, 0));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Up");
	inputSlots_[1]->SetVariableName("UP");
	inputSlots_[1]->SetDescription("Optional Up direction.");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDefaultValue(Vector3::UP);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Transform");
	inputSlots_[2]->SetVariableName("T");
	inputSlots_[2]->SetDescription("Option transform to modify");
	inputSlots_[2]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[2]->DefaultSet();


	outputSlots_[0]->SetName("Rotation Transform");
	outputSlots_[0]->SetVariableName("T");
	outputSlots_[0]->SetDescription("Rotation transformation out");
	outputSlots_[0]->SetVariantType(VariantType::VAR_MATRIX3X4);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Geometry_LookAt::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Variant pVal = inSolveInstance[0];
	Variant tVal = inSolveInstance[2];

	Vector3 up = inSolveInstance[1].GetVector3();

	if (pVal.GetType() != VAR_VECTOR3)
	{
		outSolveInstance[0] = Variant();
		return;
	}

	Matrix3x4 transform = Matrix3x4::IDENTITY;

	if (tVal.GetType() != VAR_MATRIX3X4)
	{
		outSolveInstance[0] = Variant();
		return;
	}

	transform = tVal.GetMatrix3x4();

	Vector3 startPt = transform.Translation();
	Vector3 targetPt = pVal.GetVector3();

	Quaternion rot;
	rot.FromLookRotation(targetPt - startPt, up);

	transform.SetRotation(rot.RotationMatrix());

	//Quaternion rot(rotationMatrix_out);
	outSolveInstance[0] = transform;

}
