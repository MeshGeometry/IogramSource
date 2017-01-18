#pragma once

#include <Urho3D/Core/Variant.h>

namespace Geomlib {
	bool MeshTetrahedralize(
		const Urho3D::Variant& meshIn,
		float maxVolume,
		Urho3D::Variant& meshOut
	);
}