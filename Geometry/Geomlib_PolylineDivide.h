#pragma once

#include <Urho3D/Container/Vector.h>
#include <Urho3D/Core/Variant.h>

namespace Geomlib {

bool PolylineDivide(
	const Urho3D::Variant& polylineIn,
	int n,
	Urho3D::Variant& polylineOut
);

}