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


#include "Geometry_Rotation.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Geometry_Rotation::iconTexture = "Textures/Icons/Geometry_Rotation.png";


Geometry_Rotation::Geometry_Rotation(Urho3D::Context* context) : IoComponentBase(context, 2, 2)
{
	SetName("ConstructRotation");
	SetFullName("Construct Rotation Quaternion");
	SetDescription("Construct a rotation quaternion from axis and angle");
	SetGroup(IoComponentGroup::TRANSFORM);
	SetSubgroup("Matrix");

	Variant y_unit = Vector3(0.0f, 1.0f, 0.0f);
	Variant default_rot = Matrix3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	inputSlots_[0]->SetName("Axis");
	inputSlots_[0]->SetVariableName("A");
	inputSlots_[0]->SetDescription("Axis (unit vector)");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDefaultValue(y_unit);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Angle");
	inputSlots_[1]->SetVariableName("R");
	inputSlots_[1]->SetDescription("Angle of rotation (in degrees)");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDefaultValue(0.0f);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Rotation Quaternion");
	outputSlots_[0]->SetVariableName("Q");
	outputSlots_[0]->SetDescription("Rotation quaternion out");
	outputSlots_[0]->SetVariantType(VariantType::VAR_QUATERNION);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Rotation Transform");
	outputSlots_[1]->SetVariableName("T");
	outputSlots_[1]->SetDescription("Rotation transformation out");
	outputSlots_[1]->SetVariantType(VariantType::VAR_MATRIX3X4);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Geometry_Rotation::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	// getting the rotation matrix from axis and angle: 
	// https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle

	///////////////////
	// VERIFY & EXTRACT

	// Verify slot 0
	if (inSolveInstance[0].GetType() != VariantType::VAR_VECTOR3) {
		URHO3D_LOGWARNING("A must be type Vector3.");
		outSolveInstance[0] = Variant();
		return;
	}
	Vector3 A = inSolveInstance[0].GetVector3();
	float x = A.x_;
	float y = A.y_;
	float z = A.z_;
	// Verify slot 1
	VariantType type1 = inSolveInstance[1].GetType();
	if (!(type1 == VariantType::VAR_FLOAT || type1 == VariantType::VAR_INT)) {
		URHO3D_LOGWARNING("R must be a valid float or integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	float a = inSolveInstance[1].GetFloat();
	float s = sin(a);
	float c = 1 - cos(a);

	///////////////////
	// COMPONENT'S WORK

	float r00 = cos(a) + x*x*c;
	float r01 = x*y*c - z*s;
	float r02 = x*z*c + y*s;
	float r10 = y*x*c + z*s;
	float r11 = cos(a) + y*y*c;
	float r12 = y*z*c - x*s;
	float r20 = z*x*c - y*s;
	float r21 = z*y*c + x*s;
	float r22 = cos(a) + z*z*c;


	//Matrix3 rotationMatrix_out = Matrix3(r00, r01, r02, r10, r11, r12, r20, r21, r22);

	Quaternion aaRot = Quaternion(a, A);
	Matrix3x4 mat(aaRot.RotationMatrix());

	//Matrix3 rotationMatrix_out = Matrix3(r00, r01, r02, r10, r11, r12, r20, r21, r22);

	/////////////////
	// ASSIGN OUTPUTS
	
	//Quaternion rot(rotationMatrix_out);
	outSolveInstance[0] = aaRot;
	outSolveInstance[1] = mat;
}