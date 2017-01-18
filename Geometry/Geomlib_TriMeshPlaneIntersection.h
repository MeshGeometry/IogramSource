#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Vector3.h>

namespace Geomlib {

bool TriMeshPlaneIntersection(
	const Urho3D::Variant& mesh,
	const Urho3D::Vector3& point,
	const Urho3D::Vector3& normal,
	Urho3D::Variant& mesh_normal_side,
	Urho3D::Variant& mesh_non_normal_side
);

}