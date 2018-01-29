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


#pragma once

#include <Urho3D/Core/Variant.h>

namespace Geomlib {

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
void RemoveDuplicates(
	const Urho3D::Vector<Urho3D::Variant>& vertexList,
	Urho3D::Variant& vertices,
	Urho3D::Variant& indices
);

void RemoveDuplicates(
	const Urho3D::Vector<Urho3D::Vector3>& vertexListIn,
	Urho3D::Variant& vertices,
	Urho3D::Variant& faces
);

void NMesh_RemoveDuplicates(
	const Urho3D::Vector<Urho3D::Vector3>& vertexListIn,
	const Urho3D::VariantVector& structuredFaceList,
	Urho3D::Variant& vertices,
	Urho3D::Variant& faces
);

void NMesh_WeldFaces(
	const Urho3D::VariantVector& structuredFaceList,
	Urho3D::VariantVector& weldedFaceList
);

void TriMesh_WeldFaces(
	const Urho3D::VariantVector& faceList,
	Urho3D::VariantVector& weldedFaceList
);

}