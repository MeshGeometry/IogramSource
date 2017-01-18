#include "Geomlib_TriMeshWindow.h"

#include <iostream>

#include <Urho3D/Core/Variant.h>

#include "Geomlib_Incenter.h"
#include "TriMesh.h"

using Urho3D::Variant;
using Urho3D::VariantMap;
using Urho3D::VariantVector;
using Urho3D::Vector3;

bool Geomlib::TriMeshWindow(
	const Urho3D::Variant& meshIn,
	float t,
	Urho3D::Variant& meshOut
)
{
	// t: verify in range
	if (t <= 0.0f || t >= 1.0f) {
		std::cerr << "ERROR: Geomlib:Window --- t out of recommended range\n";
		meshOut = Variant();
		return false;
	}

	// meshIn: verify and parse
	if (!TriMesh_Verify(meshIn)) {
		std::cerr << "ERROR: Geomlib::Window --- meshIn unverified\n";
		meshOut = Variant();
		return false;
	}
	VariantMap meshMap = meshIn.GetVariantMap();
	VariantVector vertexList = meshMap["vertices"].GetVariantVector();
	VariantVector faceList = meshMap["faces"].GetVariantVector();

	VariantVector newVertexList;
	VariantVector newFaceList;
	
	// perform windowing on each face
	for (unsigned i = 0; i < faceList.Size(); i += 3) {
		unsigned i0 = faceList[i].GetUInt();
		unsigned i1 = faceList[i + 1].GetUInt();
		unsigned i2 = faceList[i + 2].GetUInt();
		Vector3 A = vertexList[i0].GetVector3();
		Vector3 B = vertexList[i1].GetVector3();
		Vector3 C = vertexList[i2].GetVector3();

		Vector3 I = Geomlib::Incenter(A, B, C);

		newVertexList.Push(Variant(A + t * (I - A)));
		newVertexList.Push(Variant(B + t * (I - B)));
		newVertexList.Push(Variant(C + t * (I - C)));

		newFaceList.Push(Variant(i));
		newFaceList.Push(Variant(i + 1));
		newFaceList.Push(Variant(i + 2));
	}

	Variant newVertices(newVertexList);
	Variant newFaces(newFaceList);
	meshOut = TriMesh_Make(newVertices, newFaces);
	return true;
}