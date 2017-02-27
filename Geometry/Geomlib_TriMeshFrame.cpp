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


#include "Geomlib_TriMeshFrame.h"

#include <iostream>

#include <Urho3D/Core/Variant.h>

#include "Geomlib_Incenter.h"
#include "TriMesh.h"

using Urho3D::Vector3;
using Urho3D::Variant;
using Urho3D::VariantVector;
using Urho3D::VariantMap;

bool Geomlib::TriMeshFrame(
	const Urho3D::Variant& meshIn,
	float t,
	Urho3D::Variant& meshOut
)
{
	// t: verify in range
	if (t <= 0.0f || t >= 1.0f) {
		std::cerr << "ERROR: Geomlib:Frame --- t out of range\n";
		meshOut = Variant();
		return false;
	}

	// meshIn: verify and parse
	if (!TriMesh_Verify(meshIn)) {
		std::cerr << "ERROR: Geomlib::Frame --- meshIn unverified\n";
		meshOut = Variant();
		return false;
	}
	VariantMap meshMap = meshIn.GetVariantMap();
	const VariantVector vertexList = meshMap["vertices"].GetVariantVector();
	const VariantVector faceList = meshMap["faces"].GetVariantVector();
	VariantVector newFaceList;
	VariantVector newVertexList = vertexList;

	unsigned vertexCount = vertexList.Size();
	unsigned newVertexCount = 0;

	// perform windowing on each face
	for (unsigned i = 0; i < faceList.Size(); i += 3) {
		unsigned i0 = faceList[i].GetUInt();
		unsigned i1 = faceList[i + 1].GetUInt();
		unsigned i2 = faceList[i + 2].GetUInt();
		Vector3 A = vertexList[i0].GetVector3();
		Vector3 B = vertexList[i1].GetVector3();
		Vector3 C = vertexList[i2].GetVector3();

		Vector3 I = Geomlib::Incenter(A, B, C);

		Vector3 AA = A + t * (I - A);
		Vector3 BB = B + t * (I - B);
		Vector3 CC = C + t * (I - C);

		// j will be the index into vertexList of AA
		unsigned j = vertexCount + newVertexCount;
		newVertexList.Push(Variant(AA));
		newVertexList.Push(Variant(BB));
		newVertexList.Push(Variant(CC));
		newVertexCount += 3;

		// i0, i1, i2: indices of A, B, C
		// j, j + 1, j + 2: indices of AA, BB, CC

		// first new face
		newFaceList.Push(j);
		newFaceList.Push(i0);
		newFaceList.Push(i1);
		// second new face
		newFaceList.Push(j);
		newFaceList.Push(i1);
		newFaceList.Push(j + 1);
		// third new face
		newFaceList.Push(j + 1);
		newFaceList.Push(i1);
		newFaceList.Push(i2);
		// fourth new face
		newFaceList.Push(j + 1);
		newFaceList.Push(i2);
		newFaceList.Push(j + 2);
		// fifth new face
		newFaceList.Push(j + 2);
		newFaceList.Push(i2);
		newFaceList.Push(i0);

		// sixth new face
		newFaceList.Push(j + 2);
		newFaceList.Push(i0);
		newFaceList.Push(j);
	}

	Variant vertices(newVertexList);
	Variant faces(newFaceList);
	meshOut = TriMesh_Make(vertices, faces);
	return true;
}