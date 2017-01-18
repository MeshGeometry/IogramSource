#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Vector3.h>

namespace Geomlib {

	// Inputs
	//   mesh: mesh data stored in Urho3D::Variant
	//   q: query point
	// Outputs
	//   index: index into faceList of face closest to query point q
	//   p: coordinates of point on mesh closest to query point q
	bool TriMeshClosestPoint(
		const Urho3D::Variant& mesh,
		const Urho3D::Vector3 q,
		int& index, Urho3D::Vector3& p
	);
}