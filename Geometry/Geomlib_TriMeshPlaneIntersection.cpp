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


#include "Geomlib_TriMeshPlaneIntersection.h"

#include <assert.h>

#include "TriMesh.h"
#include "Geomlib_RemoveDuplicates.h"

namespace {

using Urho3D::Vector;
using Urho3D::Vector3;

typedef Vector<Vector3> tri_;

const double kSmallNum = 0.00000001;

// Inputs:
//   tri: stores 3 vertices of the triangle
//   point, normal: point and normal defining the plane
// Outputs:
//   num_normal_side: number of vertices on same side of plane as normal (vertices inside plane are counted here)
//   num_non_normal_side: number of vertices on opposite side of plane as normal
void TriVertexSplit(
	const tri_& tri,
	const Vector3& point,
	const Vector3& normal,
	int& num_normal_side,
	int& num_non_normal_side
)
{
	if (tri.Size() != 3) {
		// invalid triangle
		return;
	}

	num_normal_side = 0;
	num_non_normal_side = 0;

	for (int i = 0; i < 3; ++i) {

		Vector3 w = tri[i] - point;
		float val = w.DotProduct(normal);
		if (val >= 0) {
			++num_normal_side;
		}
		else {
			++num_non_normal_side;
		}
	}
	//assert(num_normal_side + num_non_normal_side == 3);
}

// Stores vertices v0, v1, v2 as triangle tri.
void PushVertsOntoTri(
	const Vector3& v0,
	const Vector3& v1,
	const Vector3& v2,
	tri_& tri
)
{
	tri.Push(v0); tri.Push(v1); tri.Push(v2);
}

// Outputs:
//   t: If there is a unique intersection, it is: (1 - t) * A + t * B
// Returns:
//   0: 0  intersection points
//   1: 1 intersection point
//   2: segment lies inside plane
int SegmentPlaneIntersection(
	const Vector3& A,
	const Vector3& B,
	const Vector3& P,
	const Vector3& N,
	float& t
)
{
	Vector3 U = B - A;
	Vector3 W = A - P;

	float dd = N.DotProduct(U);
	float nn = -1 * N.DotProduct(W);

	if (abs(dd) < kSmallNum) {
		if (abs(nn) < kSmallNum) {
			return 2; // segment lies in plane
		}
		else {
			return 0; // segment is parallel to plane but outside plane
		}
	}

	float sI = nn / dd;
	if (sI < 0 || sI > 1) {
		return 0; // intersection occurs beyond segment
	}

	t = sI;
	return 1;
}

// Preconditions:
//   (1) tri has exactly 2 vertices on normal side of plane (normal side includes inside plane) and 1 vertex on non-normal side of plane.
//       TriVertexSplit (above) can be used to confirm this condition.
// Inputs:
//   tri: triangle being split
//   P, N: point and normal of plane
// Outputs:
//   w0, w1: Store coordinates of the 2 vertices on normal side of plane.
//   w2: Stores coordinates of the 1 vertex on non-normal side of plane.
//   w3: Stores coordinates of edge [w0, w2] intersection with plane.
//   w4: Stores coordinates of edge [w1, w2] intersection with plane.
// Postcondition:
//   (1) Triangle [w0, w1, w2] has the same orientation as tri.
void TriPlaneIntersection_aux(
	const tri_& tri,
	const Vector3& P,
	const Vector3& N,
	Vector3& w0,
	Vector3& w1,
	Vector3& w2,
	Vector3& w3,
	Vector3& w4
)
{
	//assert(tri.Size() == 3);
	if (tri.Size() != 3) {
		return;
	}

	Vector<int> normal_side_indices, non_normal_side_indices;
	for (int i = 0; i < 3; ++i) {

		Vector3 v = tri[i];
		Vector3 w = v - P;
		float val = w.DotProduct(N);
		if (val >= 0) {
			normal_side_indices.Push(i);
		}
		else {
			non_normal_side_indices.Push(i);
		}
	}
	if (normal_side_indices.Size() != 2 || non_normal_side_indices.Size() != 1) {
		//
		return;
	}
	//assert(normal_side_indices.Size() == 2 && non_normal_side_indices.Size() == 1);

	int ind = non_normal_side_indices[0];
	if (ind == 0) {
		w0 = tri[1];
		w1 = tri[2];
		w2 = tri[0];
	}
	else if (ind == 1) {
		w0 = tri[2];
		w1 = tri[0];
		w2 = tri[1];
	}
	else if (ind == 2) {
		w0 = tri[0];
		w1 = tri[1];
		w2 = tri[2];
	}

	float t;
	int flag = SegmentPlaneIntersection(w0, w2, P, N, t);
	int num_intersections = 0;
	if (flag == 1) {
		w3 = (1 - t) * w0 + t * w2;
		++num_intersections;
	}
	flag = SegmentPlaneIntersection(w1, w2, P, N, t);
	if (flag == 1) {
		w4 = (1 - t) * w1 + t * w2;
		++num_intersections;
	}
	//assert(num_intersections == 2);
}

void TriPlaneIntersection(
	const tri_& tri,
	const Vector3& P,
	const Vector3& N,
	Vector<tri_>& normal_side_tris,
	Vector<tri_>& non_normal_side_tris
)
{
	int num_normal_side = 0;
	int num_non_normal_side = 0;
	TriVertexSplit(tri, P, N, num_normal_side, num_non_normal_side);
	int total_verts = num_normal_side + num_non_normal_side;
	if (total_verts != 3) {
		// intersection failed
		return;
	}

	// 3 verts on one side of plane
	if (num_normal_side == 3) {
		normal_side_tris.Push(tri);
	}
	else if (num_non_normal_side == 3) {
		non_normal_side_tris.Push(tri);
	}

	// 2 verts on one side, 1 on the other
	if (num_non_normal_side == 1 || num_non_normal_side == 2) {

		Vector3 w0, w1, w2, w3, w4;

		if (num_non_normal_side == 1) {
			TriPlaneIntersection_aux(tri, P, N, w0, w1, w2, w3, w4);
			tri_ non_norm, norm_1, norm_2;
			//tri_ tri_nns_1, tri_nns_2, tri_ns_1;
			PushVertsOntoTri(w0, w1, w3, norm_1);
			PushVertsOntoTri(w3, w1, w4, norm_2);
			PushVertsOntoTri(w3, w4, w2, non_norm);
			normal_side_tris.Push(norm_1);
			normal_side_tris.Push(norm_2);
			non_normal_side_tris.Push(non_norm);
		}
		else if (num_non_normal_side == 2) {
			TriPlaneIntersection_aux(tri, P, -1 * N, w0, w1, w2, w3, w4);
			tri_ non_norm_1, non_norm_2, norm;
			//tri_ tri_ns_1, tri_ns_2, tri_nns_1;
			PushVertsOntoTri(w0, w1, w3, non_norm_1);
			PushVertsOntoTri(w3, w1, w4, non_norm_2);
			PushVertsOntoTri(w3, w4, w2, norm);
			non_normal_side_tris.Push(non_norm_1);
			non_normal_side_tris.Push(non_norm_2);
			normal_side_tris.Push(norm);
		}
	}
}

}; // namespace

bool Geomlib::TriMeshPlaneIntersection(
	const Urho3D::Variant& mesh,
	const Urho3D::Vector3& point,
	const Urho3D::Vector3& normal,
	Urho3D::Variant& mesh_normal_side,
	Urho3D::Variant& mesh_non_normal_side
)
{
	using Urho3D::Variant;
	using Urho3D::VariantVector;
	using Urho3D::Vector;

	// Verify mesh and extract required mesh data
	if (!TriMesh_Verify(mesh)) {
		return false;
	}
	VariantVector vertex_list = TriMesh_GetVertexList(mesh);
	VariantVector face_list = TriMesh_GetFaceList(mesh);
	// Verify normal is not the zero Vector3
	if (!(normal.LengthSquared() > 0.0f)) {
		return false;
	}

	// Store two bags of triangles
	Vector<tri_> normal_side_tris;
	Vector<tri_> non_normal_side_tris;

	// Loop over faces, splitting each one
	for (unsigned i = 0; i < face_list.Size(); i += 3) {

		tri_ tri;
		int i0 = face_list[i].GetInt();
		int i1 = face_list[i + 1].GetInt();
		int i2 = face_list[i + 2].GetInt();
		tri.Push(vertex_list[i0].GetVector3());
		tri.Push(vertex_list[i1].GetVector3());
		tri.Push(vertex_list[i2].GetVector3());


		TriPlaneIntersection(
			tri,
			point,
			normal,
			normal_side_tris,
			non_normal_side_tris
		);
	}

	// Reconstruct normal side mesh from normal side bag of triangles
	Vector<Vector3> ns_vertex_list;
	for (unsigned i = 0; i < normal_side_tris.Size(); ++i) {
		tri_ tri = normal_side_tris[i];
		ns_vertex_list.Push(tri[0]);
		ns_vertex_list.Push(tri[1]);
		ns_vertex_list.Push(tri[2]);
	}
	Variant ns_vertices, ns_faces;
	RemoveDuplicates(ns_vertex_list, ns_vertices, ns_faces);
	mesh_normal_side = TriMesh_Make(ns_vertices, ns_faces);

	// Reconstruct non-normal side mesh from non-normal side bag of triangles
	Vector<Vector3> nns_vertex_list;
	for (unsigned i = 0; i < non_normal_side_tris.Size(); ++i) {
		tri_ tri = non_normal_side_tris[i];
		nns_vertex_list.Push(tri[0]);
		nns_vertex_list.Push(tri[1]);
		nns_vertex_list.Push(tri[2]);
	}
	Variant nns_vertices, nns_faces;
	RemoveDuplicates(nns_vertex_list, nns_vertices, nns_faces);
	mesh_non_normal_side = TriMesh_Make(nns_vertices, nns_faces);

	return true;
}
