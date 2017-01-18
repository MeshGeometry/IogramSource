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