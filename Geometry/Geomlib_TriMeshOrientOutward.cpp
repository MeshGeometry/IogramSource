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

#include "Geomlib_TriMeshOrientOutward.h"

#include <iostream>

#include <Urho3D/Core/Variant.h>

#include "TriMesh.h"

using namespace Urho3D;

bool Geomlib::TriMeshOrientOutward(
	const Urho3D::Variant& mesh_in,
	Urho3D::Variant& mesh_out
)
{
	if (!TriMesh_Verify(mesh_in)) {
		return false;
	}

	VariantVector normals = TriMesh_ComputeFaceNormals(mesh_in);
	if (normals.Size() == 0) return false;

	Vector3 n0 = normals[0].GetVector3();
	float m = n0.y_;
	float M = n0.y_;
	float m_ = -n0.y_;
	float M_ = -n0.y_;

	for (unsigned i = 1; i < normals.Size(); ++i) {
		Vector3 n = normals[i].GetVector3();

		float y = n.y_;

		if (y < m) {
			m = y;
		}

		if (y > M) {
			M = y;
		}

		if (-y < m_) {
			m_ = -y;
		}

		if (-y > M_) {
			M_ = -y;
		}
	}

	// m, M are min and max, respectively, of normals' y coords

	// m_, M_ are min and max, respectively, of negative normals' y coords

	if (M_ > M) { // means M_ (i.e., negative) corresponds to outward orientation

		VariantVector vertex_list = TriMesh_GetVertexList(mesh_in);
		VariantVector face_list = TriMesh_GetFaceList(mesh_out);
		VariantVector new_face_list;

		for (int i = 0; i < face_list.Size(); i += 3) {
			new_face_list.Push(face_list[i].GetInt());
			new_face_list.Push(face_list[i + 2].GetInt());
			new_face_list.Push(face_list[i + 1].GetInt());
		}
		std::cout << "Geomlib_TriMeshOrientOutward --- mesh was flipped" << std::endl;
		mesh_out = TriMesh_Make(vertex_list, new_face_list);
	}
	else {
		std::cout << "Geomlib_TriMeshOrientOutward --- mesh was not flipped" << std::endl;
		mesh_out = mesh_in;
	}

	return true;
}