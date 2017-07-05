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


#include "Geomlib_TriMeshClosestPoint.h"

#include <iostream>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Vector3.h>

#include "ConversionUtilities.h"

#include "Geomlib_ClosestPoint.h"
#include "TriMesh.h"

using namespace Urho3D;

// IN PROGRESS, NOT DONE * IN PROGRESS, NOT DONE
// Inputs
//   mesh: mesh data stored in Urho3D::Variant
//   q: query point
// Outputs
//   index: index into faceList of face closest to query point q
//   p: coordinates of point on mesh closest to query point q
bool Geomlib::TriMeshClosestPoint(const Variant& mesh, const Vector3 q, int& index, Vector3& p)
{
	if (!TriMesh_Verify(mesh)) {
		return false;
	}
	VariantMap meshMap = mesh.GetVariantMap();

	// mesh data guaranteed
	Vector<Variant> vertexList = meshMap["vertices"].GetVariantVector();
	Vector<Variant> faceList = meshMap["faces"].GetVariantVector();

	unsigned numVertices = vertexList.Size();
	assert(numVertices > 0);
	unsigned numFaces = faceList.Size() / 3;
	assert(numFaces > 0);

	float* sqDistances = new float[numFaces];
	Vector<Vector3> perFaceClosePoints;

	for (unsigned i = 0; i < faceList.Size(); i += 3) {

		unsigned faceIndex = i / 3;

		// these lookups should be guaranteed by ExtractMeshData --- no safety checks needed!
		unsigned i0 = faceList[i].GetInt();
		unsigned i1 = faceList[i + 1].GetInt();
		unsigned i2 = faceList[i + 2].GetInt();
		Vector3 v0 = vertexList[i0].GetVector3();
		Vector3 v1 = vertexList[i1].GetVector3();
		Vector3 v2 = vertexList[i2].GetVector3();

		Vector3 r = Geomlib::TriangleClosestPoint(v0, v1, v2, q);
		sqDistances[faceIndex] = (r - q).LengthSquared();
		perFaceClosePoints.Push(r);
	}

	float minSqDistance = sqDistances[0];
	unsigned minFaceIndex = 0;
	for (unsigned f = 1; f < numFaces; ++f) {
		float sqDistance = sqDistances[f];
		if (sqDistance < minSqDistance) {
			minSqDistance = sqDistance;
			minFaceIndex = f;
		}
	}

	delete[] sqDistances;

	index = (int)minFaceIndex;
	p = perFaceClosePoints[minFaceIndex];
	return true;
}

bool Geomlib::TriMeshPerVertexClosestPoint(
	const Urho3D::Variant& mesh,
	const Urho3D::Variant& target_mesh,
	Urho3D::VariantVector& closest_points
)
{
	VariantVector vertex_list = TriMesh_GetVertexList(mesh);
	closest_points.Clear();

	for (int i = 0; i < vertex_list.Size(); ++i) {

		Vector3 v = vertex_list[i].GetVector3();

		Vector3 p;
		int f = -1;
		TriMeshClosestPoint(target_mesh, v, f, p);

		closest_points.Push(p);
	}

	return true;
}