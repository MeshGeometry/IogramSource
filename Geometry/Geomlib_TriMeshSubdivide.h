#pragma once

#include <Urho3D/Core/Variant.h>

namespace Geomlib {
	bool TriMeshSubdivide(
		const Urho3D::Variant& meshIn,
		int steps,
		Urho3D::Variant& meshOut
	);
}