#pragma once

#include <Urho3D/Core/Variant.h>

namespace Geomlib {
	bool PolylineBlend(
		const Urho3D::Variant& polyline1,
		const Urho3D::Variant& polyline2,
		Urho3D::Variant& mesh
	);
}