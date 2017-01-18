#pragma once

#include <Urho3D/Math/Vector3.h>

namespace Geomlib {

	static float EPSILON = 0.00000001f;

	bool RayTriangleIntersection(
		const Urho3D::Vector3& A,
		const Urho3D::Vector3& B,
		const Urho3D::Vector3& C,
		const Urho3D::Vector3& O,
		const Urho3D::Vector3& D,
		float& s
	);
}