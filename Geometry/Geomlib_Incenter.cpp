#include "Geomlib_Incenter.h"

#include <Urho3D/Math/Vector3.h>

// Returns the incenter of triangle ABC.
Urho3D::Vector3 Geomlib::Incenter(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& C
)
{
	float a = (B - C).Length();
	float b = (C - A).Length();
	float c = (A - B).Length();

	float s = a + b + c;
	if (s == 0.0f) { // use epsilon?
		// all lengths are 0, so A, B, C are all the same point
		return A;
	}

	return (1.0f / s) * (a * A + b * B + c * C);
}