#pragma once

#include <Urho3D/Container/Vector.h>
#include <Urho3D/Core/Variant.h>

// IN PROGRESS, NOT READY!
namespace Geomlib {
	Urho3D::Variant RebuildPolyline(
		const Urho3D::Variant& polylineIn,
		int numSegments
	);
}