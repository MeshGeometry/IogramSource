#pragma once

#include <Urho3D/Core/Variant.h>

namespace Geomlib {
	bool TriMeshThicken(
		const Urho3D::Variant& meshIn,
		float d,
		Urho3D::Variant& meshOut
	);
}