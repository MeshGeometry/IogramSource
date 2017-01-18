#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>

namespace Geomlib {
	Urho3D::Vector<Urho3D::Variant> TransformVertexList(
		const Urho3D::Matrix3x4& T,
		const Urho3D::Vector<Urho3D::Variant>& vertexList
	);
}