#pragma once

#include <Urho3D/Math/Vector3.h>

namespace Geomlib {
	// Returns XYZ of a geodetic coordinate center at refLat/Lon.
	Urho3D::Vector3 GeoToXYZ(
		double refLat,
		double refLon,
		double lat,
		double lon
		);
}