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


#include "Geomlib_ConstructTransform.h"

#include <Urho3D/IO/Log.h>

using Urho3D::Matrix3x4;
using Urho3D::Vector3;
using Urho3D::Quaternion;
using Urho3D::VariantType;
using Urho3D::Variant;

Matrix3x4 Geomlib::ConstructTransform(Variant var_in)
{
	Vector3 pos = Vector3::ZERO;
	Quaternion rot = Quaternion::IDENTITY;
	Vector3 scale = Vector3::ONE;

	Urho3D::VariantType in_type = var_in.GetType();

	if (in_type == VariantType::VAR_MATRIX3X4)
		return var_in.GetMatrix3x4();

	else if (in_type == VariantType::VAR_FLOAT)
	{
		float scale_mult = var_in.GetFloat();
		scale = scale_mult*Vector3::ONE;
	}
	else if (in_type == VariantType::VAR_VECTOR3)
	{
		pos = var_in.GetVector3();
	}
	else if (in_type == VariantType::VAR_QUATERNION)
	{
		rot = var_in.GetQuaternion();
	}
	else
	{
		URHO3D_LOGWARNING("No valid transform was constructed! Using identity transform.");
	}
	return Matrix3x4(pos, rot, scale);
}

Urho3D::Matrix3x4 Geomlib::ConstructTransform(Urho3D::Variant var_in, Urho3D::Matrix3x4 source)
{
	Vector3 pos = source.Translation();
	Quaternion rot = source.Rotation();
	Vector3 scale = source.Scale();

	Urho3D::VariantType in_type = var_in.GetType();

	if (in_type == VariantType::VAR_MATRIX3X4)
		return var_in.GetMatrix3x4();

	else if (in_type == VariantType::VAR_FLOAT)
	{
		float scale_mult = var_in.GetFloat();
		scale = scale_mult*Vector3::ONE;
	}
	else if (in_type == VariantType::VAR_VECTOR3)
	{
		pos = var_in.GetVector3();
	}
	else if (in_type == VariantType::VAR_QUATERNION)
	{
		rot = var_in.GetQuaternion();
	}
	else
	{
		URHO3D_LOGWARNING("No valid transform was constructed! Using identity transform.");
	}
	return Matrix3x4(pos, rot, scale);
}