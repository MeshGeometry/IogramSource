#include "Geomlib_TriMeshClosestPoint.h"

#include <iostream>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Vector3.h>

#include "ConversionUtilities.h"

#include "Geomlib_ClosestPoint.h"
#include "TriMesh.h"

using Urho3D::Vector;
using Urho3D::Vector3;
using Urho3D::Variant;
using Urho3D::VariantMap;


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