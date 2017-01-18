#pragma once

#include <Urho3D/Core/Variant.h>

namespace Geomlib {
	bool TriMeshFrame(
		const Urho3D::Variant& meshIn,
		float t,
		Urho3D::Variant& meshOut
	);
}