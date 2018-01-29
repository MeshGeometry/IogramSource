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


#include "Maths_ConstructTransform.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include "Geomlib_ConstructTransform.h"

using namespace Urho3D;

String Maths_ConstructTransform::iconTexture = "Textures/Icons/Maths_ConstructTransform.png";

/*
WARNING: totally ripping off GH's Series node, trying to match the behavior.
*/
Maths_ConstructTransform::Maths_ConstructTransform(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("ConTransform");
	SetFullName("Construct Transform");
	SetDescription("Construct transform from position, scale, and rotation");
	SetGroup(IoComponentGroup::TRANSFORM);
	SetSubgroup("Matrix");

	inputSlots_[0]->SetName("Position");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Vector3 position");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDefaultValue(Variant(Vector3::ZERO));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Scale");
	inputSlots_[1]->SetVariableName("S");
	inputSlots_[1]->SetDescription("Vector3 or float scale");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDefaultValue(Variant(Vector3::ONE));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Rotation");
	inputSlots_[2]->SetVariableName("R");
	inputSlots_[2]->SetDescription("Quaternion for rotation");
	inputSlots_[2]->SetVariantType(VariantType::VAR_QUATERNION);
	inputSlots_[2]->SetDefaultValue(Variant(Quaternion::IDENTITY));
	inputSlots_[2]->DefaultSet();

	//inputSlots_[2]->SetName("Rotation");
	//inputSlots_[2]->SetVariableName("R");
	//inputSlots_[2]->SetDescription("Rotation Matrix");
	//inputSlots_[2]->SetVariantType(VariantType::VAR_MATRIX3);
	//inputSlots_[2]->SetDefaultValue(Variant(Matrix3::IDENTITY));
	//inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Transform");
	outputSlots_[0]->SetVariableName("T");
	outputSlots_[0]->SetDescription("Matrix3x4 representing a Transform");
	outputSlots_[0]->SetVariantType(VariantType::VAR_MATRIX3X4); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Maths_ConstructTransform::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	/////////
	// VERIFY

	if (inSolveInstance[0].GetType() != VariantType::VAR_VECTOR3) {
		URHO3D_LOGINFO("P must be a valid Vector3.");
		outSolveInstance[0] = Variant();
		return;
	}
	if (inSolveInstance[1].GetType() != VariantType::VAR_VECTOR3 && inSolveInstance[1].GetType() != VariantType::VAR_FLOAT) {
		URHO3D_LOGINFO("S must be a valid Vector3 or float.");
		outSolveInstance[0] = Variant();
		return;
	}

	// NOTE: Quaternion can also be constructed from float and Matrix3,
	// which can be stored directly in a Variant (other Quaternion construction
	// methods would require a convention to store in a Variant)
	if (inSolveInstance[2].GetType() != VariantType::VAR_QUATERNION && inSolveInstance[2].GetType() != VariantType::VAR_VECTOR3) {
		URHO3D_LOGINFO("R must be a valid Quaternion.");
		outSolveInstance[0] = Variant();
		return;
	}

	//////////
	// EXTRACT

	Vector3 pos = inSolveInstance[0].GetVector3();
	Quaternion rot = inSolveInstance[2].GetQuaternion();

	if (inSolveInstance[2].GetType() == VariantType::VAR_VECTOR3)
	{
		Vector3 euler = inSolveInstance[2].GetVector3();
		rot.FromEulerAngles(euler.x_, euler.y_, euler.z_);
	}


    
   	Vector3 scale = Vector3::ONE;
    if (inSolveInstance[1].GetType() == VariantType::VAR_VECTOR3)
        scale = inSolveInstance[1].GetVector3();
    else if (inSolveInstance[1].GetType() == VariantType::VAR_FLOAT)
        scale = inSolveInstance[1].GetFloat()*scale;

	//make sure that scale doesn't go exactly to zero
	scale.x_ = Sign(scale.x_) * Max(0.000001f, Abs(scale.x_));
	scale.y_ = Sign(scale.y_) * Max(0.000001f, Abs(scale.y_));
	scale.z_ = Sign(scale.z_) * Max(0.000001f, Abs(scale.z_));
	///////////////////
	// COMPONENT'S WORK


	Matrix3x4 xform;
	xform.SetRotation(rot.RotationMatrix());
	xform.SetTranslation(pos);
	xform.SetScale(scale);


	outSolveInstance[0] = xform;
}
