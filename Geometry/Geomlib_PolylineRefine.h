#pragma once

#include <Urho3D/Container/Vector.h>
#include <Urho3D/Core/Variant.h>

// IN PROGRESS, NOT READY!
namespace Geomlib {
	bool PolylineRefine(
		const Urho3D::Variant& polylineIn,
		const Urho3D::Vector<float>& ts,
		Urho3D::Variant& polylineOut
	);
}