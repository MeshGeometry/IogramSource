#pragma once

#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Core/Variant.h>

// Inputs:
//   polyline: storing type VAR_VARIANTMAP, with map storing type VAR_VARIANTVECTOR under key "vertices", with those variants storing type VAR_VECTOR3;
//             >= 2 distinct points in the polyline
//   t:        0 <= t <= 1 (t will be set to an appropriate endpoint if it's outside the valid interval
// Outputs:
//   point: 3D point on the line corresponding to parameter t; start of polyline is t=0, end of polyline is t=1
// Returns true if computation was successfully carried out.
namespace Geomlib {
	bool PolylinePointFromParameter(
		const Urho3D::Variant& polyline,
		float t,
		Urho3D::Vector3& point
	);
}