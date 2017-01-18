#pragma once

#include <Urho3D/Core/Variant.h>

namespace Geomlib {
	bool TriMeshLoopSubdivide(
		const Urho3D::Variant& meshIn,
		int steps,
		Urho3D::Variant& meshOut
	);
}