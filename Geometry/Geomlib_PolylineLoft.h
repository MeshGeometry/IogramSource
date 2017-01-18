#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>

namespace Geomlib {

bool PolylineLoft(
	const Urho3D::Vector<Urho3D::Variant>& polylines,
	Urho3D::Variant& mesh
);

}