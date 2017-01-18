#pragma once

#include <Urho3D/Core/Variant.h>

namespace Geomlib {

Urho3D::Variant TriMesh_MeanCurvatureFlowStep(
	const Urho3D::Variant& tri_mesh
);

bool TriMesh_MeanCurvatureFlow(
	const Urho3D::Variant& tri_mesh,
	int num_steps,
	Urho3D::Variant& tri_mesh_out
);

}