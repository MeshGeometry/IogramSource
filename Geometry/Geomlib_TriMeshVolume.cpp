#include "Geomlib_TriMeshVolume.h"

#include "TriMesh.h"

using Urho3D::Variant;
using Urho3D::VariantVector;
using Urho3D::Vector3;

namespace {

float SignedVolumeOfTriangle(
	const Urho3D::Vector3& p1,
	const Urho3D::Vector3& p2,
	const Urho3D::Vector3& p3
)
{
	return p1.DotProduct(p2.CrossProduct(p3)) / 6.0f;
}

} // namespace

float Geomlib::TriMeshVolume(const Urho3D::Variant& tri_mesh)
{
	float volume = 0.0f;

	VariantVector vertex_list = TriMesh_GetVertexList(tri_mesh);
	VariantVector face_list = TriMesh_GetFaceList(tri_mesh);

	for (unsigned i = 0; i < face_list.Size(); i += 3) {

		int i0 = face_list[i].GetInt();
		int i1 = face_list[i + 1].GetInt();
		int i2 = face_list[i + 2].GetInt();

		volume += SignedVolumeOfTriangle(
			vertex_list[i0].GetVector3(),
			vertex_list[i1].GetVector3(),
			vertex_list[i2].GetVector3()
		);
	}

	return volume;
}