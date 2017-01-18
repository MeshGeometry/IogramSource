#pragma once

#include <Urho3D/Core/Variant.h>

namespace Geomlib {

	// construct transform: check if we have full information (Matrix3X4)
	// otherwise, construct transform from provided info
	// accepted inputs are 
	// Vector3 position
	// float scale (to be applied uniformly in all dimensions)
	// quaternion rotation

	Urho3D::Matrix3x4 ConstructTransform(Urho3D::Variant var_in);
	Urho3D::Matrix3x4 ConstructTransform(Urho3D::Variant var_in, Urho3D::Matrix3x4 source);
}