#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>

namespace Geomlib {

Urho3D::Variant JoinMeshes(
	const Urho3D::Vector<Urho3D::Variant>& mesh_list
);

}