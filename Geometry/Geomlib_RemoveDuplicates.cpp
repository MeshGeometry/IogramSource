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


#include "Geomlib_RemoveDuplicates.h"

#include <algorithm>
#include <vector>

#pragma warning(disable : 4244)

using Urho3D::Variant;
using Urho3D::Vector;
using Urho3D::Vector3;

namespace {
bool Vector3Equals(const Urho3D::Vector3& lhs, const Urho3D::Vector3& rhs)
{
	return Urho3D::Equals(lhs.x_, rhs.x_) && Urho3D::Equals(lhs.y_, rhs.y_) && Urho3D::Equals(lhs.z_, rhs.z_);
}
}

// vertexList:
//   If object is a triangle mesh, then vertexList is a triangle-by-triangle list
//   of coordinates of vertices, e.g.,
//     x0,y0,z0,x1,y1,z1,x2,y2,z2,x3,y3,z3,...
//   where v0 v1 v2 is the first face, v3 v4 v5 is the second face, and so on.
//   Vertices shared by distinct faces just get listed over and over, once for each
//   face that they're in.
//   The index array corresponding to such a triangle list would be
//     0,1,2,3,4,5,...
//   but it's clear that this is redundant so we don't need it.
//   This scheme also works for quad meshes, *regular* n-gon meshes, polylines, etc.
//   Coordinates are stored as Vector3 objects inside Variants
//   stored in the VariantVector called vertexList.
// vertices:
//   A Variant storing type VAR_VARIANTVECTOR with each Variant in the Vector
//   having type VAR_VECTOR3.
//   This represents a list of the same vertices from vertexList, but with
//   duplicates removed.
// indices:
//   A Variant storing type VAR_VARIANTVECTOR with each Variant in the Vector
//   having type VAR_INT.
//   This represents a list of indices into the list stored in vertices, and
//   in the case of a triangle mesh it is our familiar face list.
//   For a polyline, it would be a segment list, etc.
//   WARNING: result depends on input format, for e.g., a polyline's vertex list is
//     a bit different from a mesh's triangle list, i.e., for a polyline we just list
//     vertices in order v0 v1 v2 v3 so the segments are v0 v1 and then v1 v2
//     then v2 v3 etc.
//     An exact analogue to the triangle list would be to store
//       v0 v1
//       v1 v2
//       v2 v3
//       ...
//   but we don't do this, we just do v0 v1 v2, etc. So beware.
void Geomlib::RemoveDuplicates(
	const Urho3D::Vector<Urho3D::Variant>& vertexList,
	Urho3D::Variant& vertices,
	Urho3D::Variant& indices
)
{
	using Urho3D::Variant;
	using Urho3D::Vector;

	int numVerts = (int)vertexList.Size();
	std::vector<int> vertexDuplicates_(numVerts);

	for (int i = 0; i < numVerts; ++i) {
		vertexDuplicates_[i] = i; // Assume not a duplicate
		for (int j = 0; j < i; ++j) {
			if (Vector3Equals(vertexList[j].GetVector3(), vertexList[i].GetVector3())) {
				vertexDuplicates_[i] = j;
				break;
			}
		}
	}

	std::vector<int> vertexUniques_ = vertexDuplicates_;
	std::sort(vertexUniques_.begin(), vertexUniques_.end());
	vertexUniques_.erase(std::unique(vertexUniques_.begin(), vertexUniques_.end()), vertexUniques_.end());

	Vector<Variant> newVertexList;
	for (int i = 0; i < vertexUniques_.size(); ++i) {
		int k = vertexUniques_[i];
		newVertexList.Push(Variant(vertexList[k].GetVector3()));
	}
	vertices = Variant(newVertexList);

	Vector<Variant> indexList;
	for (int j = 0; j < numVerts; ++j) {
		int orig_j = vertexDuplicates_[j];
		int uniq_j = find(vertexUniques_.begin(), vertexUniques_.end(), orig_j) - vertexUniques_.begin();
		indexList.Push(Variant(uniq_j));
	}
	indices = Variant(indexList);
}

void Geomlib::RemoveDuplicates(
	const Vector<Vector3>& vertexListIn,
	Variant& vertices,
	Variant& faces
)
{
	int numVerts = (int)vertexListIn.Size();
	std::vector<int> vertexDuplicates_(numVerts);

	for (int i = 0; i < numVerts; ++i) {
		vertexDuplicates_[i] = i; // Assume not a duplicate
		for (int j = 0; j < i; ++j) {
			if (Vector3Equals(vertexListIn[j], vertexListIn[i])) {
				vertexDuplicates_[i] = j;
				break;
			}
		}
	}

	std::vector<int> vertexUniques_ = vertexDuplicates_;
	std::sort(vertexUniques_.begin(), vertexUniques_.end());
	vertexUniques_.erase(std::unique(vertexUniques_.begin(), vertexUniques_.end()), vertexUniques_.end());

	Vector<Variant> newVertexList;
	for (int i = 0; i < vertexUniques_.size(); ++i) {
		int k = vertexUniques_[i];
		newVertexList.Push(Variant(vertexListIn[k]));
	}
	vertices = Variant(newVertexList);

	Vector<Variant> faceList;
	for (int j = 0; j < numVerts; ++j) {
		int orig_j = vertexDuplicates_[j];
		int uniq_j = find(vertexUniques_.begin(), vertexUniques_.end(), orig_j) - vertexUniques_.begin();
		faceList.Push(Variant(uniq_j));
	}
	faces = Variant(faceList);
}