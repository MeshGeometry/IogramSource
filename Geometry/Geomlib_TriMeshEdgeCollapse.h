#pragma once

#include <Urho3D/Core/Variant.h>

namespace Geomlib {

Urho3D::Variant TriMesh_CollapseShortEdges(
	const Urho3D::Variant& tri_mesh,
	float collapse_threshold
);

}