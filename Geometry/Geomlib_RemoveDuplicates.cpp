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
using Urho3D::VariantMap;
using Urho3D::VariantVector;

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

	//// weld the faces
	//VariantVector weldedFaceList;
	//TriMesh_WeldFaces(faceList, weldedFaceList);

	//faces = Variant(weldedFaceList);

	faces = Variant(faceList);
}

void Geomlib::NMesh_RemoveDuplicates(
	const Urho3D::Vector<Urho3D::Vector3>& vertexListIn,
	const Urho3D::VariantVector& structuredFaceList,
	Urho3D::Variant& vertices,
	Urho3D::Variant& faces)
{
	// use the same idea as trimesh version with the vertexlistin, but use the structured face list for info about the faces

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

	// now update the nmesh face list
	Vector<Variant> faceList_raw;
	Vector<Variant> faceList;
	for (int j = 0; j < numVerts; ++j) {
		int orig_j = vertexDuplicates_[j];
		int uniq_j = find(vertexUniques_.begin(), vertexUniques_.end(), orig_j) - vertexUniques_.begin();
		faceList_raw.Push(Variant(uniq_j));
	}

	VariantVector revised_sfl;
	int counter = 0;
	for (int i = 0; i < structuredFaceList.Size(); ++i) {
		VariantMap face = structuredFaceList[i].GetVariantMap();
		VariantMap rev_face = face;
		VariantVector face_verts = face["face_vertices"].GetVariantVector();
		VariantVector rev_face_verts;
		int sz = face_verts.Size();
		faceList.Push(Variant(sz));

		for (int j = 0; j < sz; ++j) {
			int id = faceList_raw[counter + j].GetInt();
			faceList.Push(Variant(id));
			rev_face_verts.Push(Variant(id));
		}
		rev_face["face_vertices"] = rev_face_verts;
		revised_sfl.Push(Variant(rev_face));
		counter += sz;
	}

	//// weld the faces
	//VariantVector final_sfl;
	//Vector<Variant> final_faceList;
	//NMesh_WeldFaces(revised_sfl, final_sfl);

	//// now convert to the Nmesh make format
	//for (int i = 0; i < final_sfl.Size(); ++i) {
	//	VariantMap face = final_sfl[0].GetVariantMap();
	//	VariantVector face_verts = face["face_vertices"].GetVariantVector();
	//	int sz = face_verts.Size();
	//	final_faceList.Push(Variant(sz));
	//	for (int j = 0; j < sz; ++j) {
	//		int id = face_verts[j].GetInt();
	//		final_faceList.Push(Variant(id));
	//	}
	//	counter += sz;
	//}


	//faces = Variant(final_faceList);
	faces = Variant(faceList);


}

void Geomlib::NMesh_WeldFaces(const Urho3D::VariantVector & structuredFaceList, Urho3D::VariantVector & weldedFaceList)
{
	// first face is fine. 
	if (structuredFaceList.Size() == 0)
		return;

	VariantMap first = structuredFaceList[0].GetVariantMap();
	weldedFaceList.Push(Variant(first));

	for (int i = 1; i < structuredFaceList.Size(); ++i) {
		VariantMap F = structuredFaceList[i].GetVariantMap();
		VariantVector V = F["face_vertices"].GetVariantVector();
		bool unique = true;
		for (int j = 0; j < weldedFaceList.Size(); ++j) {
			VariantMap F_0 = weldedFaceList[j].GetVariantMap();
			VariantVector V_0 = F_0["face_vertices"].GetVariantVector();

			if (V_0.Size() != V.Size())
				continue;
			else {
				// check if the vert ids are the same
				int start = V[0].GetInt();
				int match = -1;
				for (int h = 0; h < V_0.Size(); ++h) {
					if (V_0[h].GetInt() == start)
						match = h;
				}
				if (match > -1) {
					int CW_counter = 1;
					int CCW_counter = 1;
					//clockwise winding
					for (int k = 1; k < V.Size(); ++k) {
						int CW = (match - k + V.Size()) % V.Size();
						if (V[k].GetInt() != V_0[CW].GetInt())
							break;
						else
							++CW_counter;
					 }
					if (CW_counter == V.Size()) {
						unique = false;
						break;
					}
					// counterclockwise winding
					for (int k = 1; k < V.Size(); ++k) {
						int CCW = (match + k) % V.Size();
 						if (V[k].GetInt() != V_0[CCW].GetInt())
							break;
						else
							++CCW_counter;
					}
					if (CCW_counter == V.Size()) {
						unique = false;
						break;
					}
				}	
			}
			if (unique == false)
				break;
		}
		if (unique == false)
			continue;
		else if (unique == true)
			weldedFaceList.Push(Variant(F));
	}

}

void Geomlib::TriMesh_WeldFaces(const Urho3D::VariantVector & faceList, Urho3D::VariantVector & weldedFaceList)
{
	//convert to nmesh format.
	VariantVector structuredFaceList;
	VariantVector weldedNFaceList;

	for (int i = 0; i < faceList.Size()/3; ++i) {
		int id = 3 * i;
		VariantVector face_ids;
		face_ids.Push(Variant(faceList[id]));
		face_ids.Push(Variant(faceList[id+1]));
		face_ids.Push(Variant(faceList[id+2]));
		VariantMap face;
		face["face_vertices"] = face_ids;
		structuredFaceList.Push(Variant(face));
	}

	NMesh_WeldFaces(structuredFaceList, weldedNFaceList);

	//convert back to trimesh
	for (int i = 0; i < weldedNFaceList.Size(); ++i) {
		VariantMap face = weldedNFaceList[i].GetVariantMap();
		VariantVector face_ids = face["face_vertices"].GetVariantVector();
		for (int j = 0; j < 3; ++j) {
			weldedFaceList.Push(face_ids[j]);
		}
	}

}
