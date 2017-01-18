#pragma once

#include <Urho3D/Core/Variant.h>

namespace Geomlib {

bool PolylinePointToTransform(
	const Urho3D::Variant& polyline,
	int i,
	Urho3D::Variant& T
);

}