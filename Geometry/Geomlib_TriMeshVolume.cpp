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