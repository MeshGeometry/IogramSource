#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>

namespace Geomlib {

	bool PolylineOffset(
		const Urho3D::Variant& polyIn,
		Urho3D::Variant& polyOut,
		float offset
		);

}