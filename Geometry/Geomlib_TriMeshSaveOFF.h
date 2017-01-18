#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/IO/File.h>

namespace Geomlib {
	bool TriMeshWriteOFF(
		const Urho3D::Variant& meshIn,
		Urho3D::File& destination
	);
}