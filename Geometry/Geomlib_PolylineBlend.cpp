#include "Geomlib_PolylineBlend.h"

#include <iostream>

#include "Geomlib_PolylineRefine.h"
#include "TriMesh.h"
#include "Polyline.h"


namespace {

using Urho3D::Vector;
using Urho3D::Variant;
using Urho3D::VariantVector;
using Urho3D::Vector3;
using Urho3D::Equals;

bool Vector3Equals(const Vector3& lhs, const Vector3& rhs)
{
	return Equals(lhs.x_, rhs.x_) && Equals(lhs.y_, rhs.y_) && Equals(lhs.z_, rhs.z_);
}

Vector<Vector3> RemoveRepeats(const Vector<Vector3>& vertexList)
{
	if (vertexList.Size() <= 1) {
		return vertexList;
	}

	Vector<Vector3> newVertexList;
	newVertexList.Push(vertexList[0]);
	for (unsigned i = 0; i < vertexList.Size() - 1; ++i) {
		if (!Vector3Equals(vertexList[i], vertexList[i + 1])) {
			newVertexList.Push(vertexList[i + 1]);
		}
	}

	return newVertexList;
}

Vector<float> ComputePolylineParams(const VariantVector& vertexList)
{
	float length = 0.0f;
	// loop over each edge [i, i + 1]
	for (int i = 0; i < vertexList.Size() - 1; ++i) {
		Vector3 seg = vertexList[i + 1].GetVector3() - vertexList[i].GetVector3();
		length += seg.Length();
	}
	if (Urho3D::Equals(length, 0.0f)) {
		// give up!
		return Vector<float>();
	}

	Vector<float> dists;

	float sublength = 0.0f;
	dists.Push(sublength);
	for (unsigned i = 0; i < vertexList.Size() - 1; ++i) {
		Vector3 seg = vertexList[i + 1].GetVector3() - vertexList[i].GetVector3();
		sublength += seg.Length();
		dists.Push(sublength);
	}

	Vector<float> ret;
	for (unsigned i = 0; i < dists.Size(); ++i) {
		ret.Push(dists[i] / length);
	}
	return ret;
}

} // namespace




bool Geomlib::PolylineBlend(
	const Urho3D::Variant& polyline1,
	const Urho3D::Variant& polyline2,
	Urho3D::Variant& mesh
)
{
	using Urho3D::Variant;
	using Urho3D::VariantType;
	using Urho3D::VariantMap;
	using Urho3D::VariantVector;
	using Urho3D::Vector;
	using Urho3D::Vector3;

	/////////////////////////
	// VERIFY INPUT POLYLINES

	if (!Polyline_Verify(polyline1)) {
		mesh = Variant();
		return false;
	}
	if (!Polyline_Verify(polyline2)) {
		mesh = Variant();
		return false;
	}

	//////////////////
	// CLEAN & EXTRACT

	Variant cleanPoly1 = Polyline_Clean(polyline1);
	VariantVector cleanVerts1 = Polyline_ComputeSequentialVertexList(polyline1);

	Variant cleanPoly2 = Polyline_Clean(polyline2);
	VariantVector cleanVerts2 = Polyline_ComputeSequentialVertexList(polyline2);

	Vector<float> params1 = ComputePolylineParams(cleanVerts1);
	Vector<float> params2 = ComputePolylineParams(cleanVerts2);

	if (params1.Size() > 0 && params2.Size() > 0) {
		Variant polyOut1;
		bool success1 = PolylineRefine(cleanPoly1, params2, polyOut1);
		if (!success1) {
			mesh = Variant();
			std::cerr << "ERROR: Geomlib::PolylineBlend --- PolylineRefine failed on polyline 1\n";
			return false;
		}

		Variant polyOut2;
		bool success2 = PolylineRefine(cleanPoly2, params1, polyOut2);
		if (!success2) {
			mesh = Variant();
			std::cerr << "ERROR: Geomlib::PolylineBlend --- PolylineRefine failed on polyline 2\n";
			return false;
		}

		VariantVector outVerts1 = Polyline_ComputeSequentialVertexList(polyOut1);
		VariantVector outVerts2 = Polyline_ComputeSequentialVertexList(polyOut2);

		if (outVerts1.Size() == outVerts2.Size()) {
			if (outVerts1.Size() <= 1) {
				std::cerr << "ERROR: Geomlib::PolylineBlend --- polys have <= 1 verts after refinement\n";
				mesh = Variant();
				return false;
			}
			VariantVector meshVertList = outVerts1;
			meshVertList.Push(outVerts2);
			VariantVector meshFaceList;
			unsigned N = outVerts1.Size();

			// Each pass processes edge [i, i + 1].
			for (unsigned i = 0; i < outVerts1.Size() - 1; ++i) {
				// first face
				meshFaceList.Push(i);
				meshFaceList.Push(i + 1);
				meshFaceList.Push(N + i + 1);
				// second face
				meshFaceList.Push(N + i + 1);
				meshFaceList.Push(N + i);
				meshFaceList.Push(i);
			}

			Variant meshVertices(meshVertList);
			Variant meshFaces(meshFaceList);
			mesh = TriMesh_Make(meshVertices, meshFaces);
			return true;
		}
		else {
			std::cerr << "ERROR: Geomlib::PolylineBlend --- polys have unequal vert num after refinement\n";
			mesh = Variant();
			return false;
		}
	}

	mesh = Variant();
	std::cerr << "ERROR: Geomlib::PolylineBlend --- params1 or params2 has size 0\n";
	return false;
}