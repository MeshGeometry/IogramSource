#include "Geomlib_TransformVertexList.h"

#include <Urho3D/Core/Variant.h>

using Urho3D::Variant;
using Urho3D::Vector;
using Urho3D::Vector3;

Urho3D::Vector<Urho3D::Variant> Geomlib::TransformVertexList(
	const Urho3D::Matrix3x4& T,
	const Urho3D::Vector<Urho3D::Variant>& vertexList
)
{
	Vector<Variant> newVertexList;

	for (unsigned i = 0; i < vertexList.Size(); ++i) {
		Vector3 v = T * vertexList[i].GetVector3();
		newVertexList.Push(v);
	}

	return newVertexList;
}