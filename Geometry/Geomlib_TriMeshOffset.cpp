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