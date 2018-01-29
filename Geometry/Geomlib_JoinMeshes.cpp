//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "Geomlib_JoinMeshes.h"

#include <iostream>

#include "TriMesh.h"
#include "NMesh.h"
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

Urho3D::Variant Geomlib::JoinNMeshes(
	const Urho3D::Vector<Urho3D::Variant>& mesh_list
)
{
	VariantVector actual_mesh_list;
	for (unsigned i = 0; i < mesh_list.Size(); ++i) {
		if (NMesh_Verify(mesh_list[i])) {
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

		VariantVector vertex_list = NMesh_GetVertexList(cur_mesh);
		for (unsigned j = 0; j < vertex_list.Size(); ++j) {

			Vector3 vertex = vertex_list[j].GetVector3();
			master_vertex_list.Push(Variant(vertex));
		}

		//faces, carefully now
		VariantVector face_list = NMesh_GetFaceList(cur_mesh);
		for (unsigned j = 0; j < face_list.Size(); ++j) {

			Urho3D::VariantMap face = face_list[j].GetVariantMap();
			Urho3D::VariantVector face_verts = face["face_vertices"].GetVariantVector();
			int sz = face_verts.Size();
			master_face_list.Push(Variant(sz));

			for (int k = 0; k < sz; ++k) {
				int face_index = face_verts[k].GetInt() + running_vertex_count;
				master_face_list.Push(Variant(face_index));
			}
		
		}

		running_vertex_count += vertex_list.Size();
	}

	return NMesh_Make(master_vertex_list, master_face_list);
}

