#pragma once

#include <Urho3D/Core/Variant.h>

namespace Geomlib {
	bool SmoothPolyline(
		const Urho3D::Variant& polylineIn,
		Urho3D::Variant& polylineOut
	);
}