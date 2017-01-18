#include "Geomlib_RayTriangleIntersection.h"

#include <Urho3D/Math/Vector3.h>

bool Geomlib::RayTriangleIntersection(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& C,
	const Urho3D::Vector3& O,
	const Urho3D::Vector3& D,
	float& s
)
{
	Urho3D::Vector3 E1 = B - A;
	Urho3D::Vector3 E2 = C - A;
	Urho3D::Vector3 P = D.CrossProduct(E2);
	float det = E1.DotProduct(P);

	if (det > -EPSILON && det < EPSILON) {
		return false;
	}

	float detInv = 1.0f / det;

	Urho3D::Vector3 T = O - A;
	float u = T.DotProduct(P) * detInv;
	if (u < 0.0f || u > 1.0f) {
		return false;
	}

	Urho3D::Vector3 Q = T.CrossProduct(E1);
	float v = D.DotProduct(Q) * detInv;
	if (v < 0.0f || u + v > 1.0f) {
		return false;
	}

	float t = E2.DotProduct(Q) * detInv;

	if (t > EPSILON) { // ray intersects triangle
		s = t;
		return true;
	}

	// ray misses triangle
	return false;
}