#pragma once

#include <Urho3D/Core/Variant.h>

namespace Geomlib {

bool TriMesh_HausdorffDistance(
	const Urho3D::Variant& mesh1,
	const Urho3D::Variant& mesh2,
	float& distance
);

}