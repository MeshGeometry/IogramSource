#pragma once

#include <Urho3D/Math/Vector3.h>

namespace Geomlib {
	// Returns the incenter of triangle ABC.
	Urho3D::Vector3 Incenter(
		const Urho3D::Vector3& A,
		const Urho3D::Vector3& B,
		const Urho3D::Vector3& C
	);
}