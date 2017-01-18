#include "Geomlib_JoinMeshes.h"

#include <iostream>

#include "TriMesh.h"
#include "Geomlib_RemoveDuplicates.h"

using Urho3D::Variant;
using Urho3D::Vector3;
using Urho3D::Vector;
using Urho3D::VariantVector;

Urho3D::Variant Geomlib::JoinMeshes(
	const Urho3D::Vector<Urho3D::Variant>& mesh_list
)
{
	VariantVector actual_mesh_list;
	for (unsigned i = 0; i < mesh_list.Size(); ++i) {
		if (TriMesh_Verify(mesh_list[i])) {
			actual_mesh_list.Push(mesh_list[i]);
		}
	}

	if (actual_mesh_list.Size() == 0) {
		return Variant();
	}

	VariantVector master_vertex_list;
	VariantVector master_face_list;
	int running_vertex_count = 0;

	for (unsigned i = 0; i < actual_mesh_list.Size(); ++i) {

		Variant cur_mesh = actual_mesh_list[i];

		VariantVector vertex_list = TriMesh_GetVertexList(cur_mesh);
		for (unsigned j = 0; j < vertex_list.Size(); ++j) {

			Vector3 vertex = vertex_list[j].GetVector3();
			master_vertex_list.Push(Variant(vertex));
		}

		VariantVector face_list = TriMesh_GetFaceList(cur_mesh);
		for (unsigned j = 0; j < face_list.Size(); ++j) {

			int face_index = face_list[j].GetInt() + running_vertex_count;
			master_face_list.Push(Variant(face_index));
		}

		running_vertex_count += vertex_list.Size();
	}

	return TriMesh_Make(master_vertex_list, master_face_list);
}