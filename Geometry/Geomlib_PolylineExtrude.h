#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>

namespace Geomlib {

	bool PolylineExtrude(
		const Urho3D::Variant& polyIn,
		Urho3D::Variant& meshOut,
		Urho3D::Vector3 dir
		);

}