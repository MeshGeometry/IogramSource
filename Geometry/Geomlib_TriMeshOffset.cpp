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


#include "Geomlib_TriMeshOffset.h"

#include <iostream>

#include <Urho3D/Core/Variant.h>

#include "TriMesh.h"

using Urho3D::Variant;
using Urho3D::VariantMap;
using Urho3D::VariantVector;
using Urho3D::Vector3;

bool Geomlib::TriMeshOffset(
	const Urho3D::Variant& meshIn,
	float d,
	Urho3D::Variant& meshOut
)
{
	if (!TriMesh_Verify(meshIn)) {
		std::cerr << "ERROR: Geomlib::Offset --- meshIn unverified\n";
		meshOut = Variant();
		return false;
	}

	// meshIn: verify and parse
	VariantMap meshMap = meshIn.GetVariantMap();
	const VariantVector vertexList = meshMap["vertices"].GetVariantVector();
	const VariantVector faceList = meshMap["faces"].GetVariantVector();

	VariantVector vertNormals = TriMesh_ComputeVertexNormals(meshIn, true);
	assert(vertNormals.Size() == vertexList.Size());

	VariantVector newVertexList;
	for (unsigned i = 0; i < vertexList.Size(); ++i) {
		Vector3 vert = vertexList[i].GetVector3();
		Vector3 unorm = vertNormals[i].GetVector3();

		Vector3 newVert = vert + d * unorm;
		newVertexList.Push(Variant(newVert));
	}

	// newVertexList is ready
	VariantVector newFaceList = faceList;
	Variant newVertices(newVertexList);
	Variant newFaces(newFaceList);
	meshOut = TriMesh_Make(newVertices, newFaces);

	return true;
}