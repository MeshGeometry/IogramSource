#pragma once

#include <Urho3D/Container/Vector.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Vector3.h>

namespace Geomlib {

void BestFitPlane(
	const Urho3D::Vector<Urho3D::Vector3>& point_cloud,
	Urho3D::Vector3& point,
	Urho3D::Vector3& normal
);

void BestFitPlane(
	const Urho3D::VariantVector& vertex_list,
	Urho3D::Vector3& point,
	Urho3D::Vector3& normal
);


}