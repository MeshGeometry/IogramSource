#pragma once

#include <Urho3D/Math/Vector3.h>

namespace Geomlib {
	// Returns point on line segment AB closest to query point Q
	// Intended to be crash proof.
	Urho3D::Vector3 SegmentClosestPoint(
		const Urho3D::Vector3& A,
		const Urho3D::Vector3& B,
		const Urho3D::Vector3& Q
	);

	// Returns point on perimeter of triangle ABC closest to query point Q
	// Intended to be crash proof.
	Urho3D::Vector3 TrianglePerimeterClosestPoint(
		const Urho3D::Vector3& A,
		const Urho3D::Vector3& B,
		const Urho3D::Vector3& C,
		const Urho3D::Vector3& Q
	);

	Urho3D::Vector3 TriangleClosestPoint(
		const Urho3D::Vector3& A,
		const Urho3D::Vector3& B,
		const Urho3D::Vector3& C,
		const Urho3D::Vector3& Q
	);
}