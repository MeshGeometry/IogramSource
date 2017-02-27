#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>

namespace Geomlib {
	Urho3D::Vector3 ComputeSunPosition(
		float year,
		float month,
		float day,
		float hour,
		int tzone,
		float latit,
		float longit
	);
}