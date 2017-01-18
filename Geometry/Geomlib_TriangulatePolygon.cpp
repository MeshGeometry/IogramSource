#include "Geomlib_TriangulatePolygon.h"
#include "TriMesh.h"
#include "Polyline.h"
#include "NMesh.h"
#include "Geomlib_BestFitPlane.h"

#pragma warning(push, 0)
#include "../contrib/poly2tri/poly2tri/poly2tri.h"
#pragma warning(pop)

#include <vector>
#include <map>

using Urho3D::Matrix3x4;
using Urho3D::Variant;
using Urho3D::VariantMap;
using Urho3D::VariantType;
using Urho3D::VariantVector;
using Urho3D::Vector3;

namespace {

int LargestMagnitudeIndex(Vector3 v)
{
	float coords[3] = { v.x_, v.y_, v.z_ };

	int max_index = 0;
	float max_sq = coords[0] * coords[0];
	for (int i = 1; i < 3; ++i) {
		if (coords[i] * coords[i] > max_sq) {
			max_index = i;
			max_sq = coords[i] * coords[i];
		}
	}

	return max_index;
}

}; // namespace

// Inputs:
//   P, N: point and unit normal on best fit plane
void GetP2TPointsFromVariant(
	const Urho3D::Variant& polyIn,
	const Urho3D::Vector3& P,
	const Urho3D::Vector3& N,
	std::vector<p2t::Point*>& p2tPoints,
	bool isHole)
{
	//get the data
	VariantMap polyMap = polyIn.GetVariantMap();
	VariantVector verts = polyMap["vertices"].GetVariantVector();
	VariantVector faces = polyMap["faces"].GetVariantVector();

	Vector3 lP, lN;
	Geomlib::BestFitPlane(verts, lP, lN);
	int max_index = LargestMagnitudeIndex(lN);

	int numVerts = verts.Size();

	//check if closed
	float endPointDistance = (verts[0].GetVector3() - verts[numVerts - 1].GetVector3()).Length();
	bool isClosed = (endPointDistance < 1.0f / Urho3D::M_LARGE_VALUE) ? true : false;

	//exit if not closed
	if (!isClosed)
		return;

	//erase the duplicate at end
	verts.Erase(numVerts - 1, 1);
	numVerts = verts.Size();

	//TODO::find the best fit transform
	Matrix3x4 bestFitFrame = Matrix3x4::IDENTITY;
	Vector3 localUp = bestFitFrame * Vector3::UP;

	//import the points to poly 
	for (int i = 0; i < numVerts; i++)
	{
		Vector3 currVert = verts[i].GetVector3();
		Vector3 fitVert = bestFitFrame.Inverse() * currVert;

		int i0 = (max_index == 0) ? 1 : 0;
		int i1 = (max_index == 2) ? 1 : 2;
		float coords[3] = { currVert.x_, currVert.y_, currVert.z_ };

		Vector3 Q = currVert;
		Vector3 proj = Q - (Q - lP).DotProduct(lN) * lN;

		//add as planar pair
		p2t::Point* p = new p2t::Point(coords[i0], coords[i1]);
		//p2t::Point* p = new p2t::Point(fitVert.x_, fitVert.z_);
		p2tPoints.push_back(p);
	}

	if (isHole)
		std::reverse(p2tPoints.begin(), p2tPoints.end());
}

void AddPointsToMap(std::map<p2t::Point*, int>& ptIndexMap, const std::vector<p2t::Point*>& p2tPoints)
{
	int numKeys = (int)ptIndexMap.size();
	for (int i = 0; i < p2tPoints.size(); i++)
	{
		ptIndexMap[p2tPoints[i]] = numKeys + i;
	}
}

bool Geomlib::TriangulatePolygon(const Urho3D::Variant& polyIn, const Urho3D::VariantVector& holes, Urho3D::Variant& meshOut)
{
	//data check
	bool dataCheck = false;
	if (polyIn.GetType() == VariantType::VAR_VARIANTMAP)
	{
		if (polyIn.GetVariantMap().Keys().Contains("vertices"))
			dataCheck = true;
	}

	if (!dataCheck)
	{
		return false;
	}



	//container for p2t points
	std::vector<p2t::Point*> p2tPoints;
	//map to get indices from points. A bit annoyting that p2t doesn't does this for you.
	std::map<p2t::Point*, int> ptIndexMap;

	GetP2TPointsFromVariant(polyIn, Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), p2tPoints, false);
	AddPointsToMap(ptIndexMap, p2tPoints);

	if (p2tPoints.size() < 3)
	{
		return false;
	}

	//track the verts of the mesh
	VariantMap polyMap = polyIn.GetVariantMap();
	VariantVector verts = polyMap["vertices"].GetVariantVector();

	bool closed = Polyline_IsClosed(polyIn);

	verts.Erase(verts.Size() - 1, 1);

	if (verts.Size() == 3)
	{
		VariantVector faces;
		faces.Resize(3);
		faces[0] = 0;
		faces[1] = 1;
		faces[2] = 2;

		meshOut = TriMesh_Make(verts, faces);
		return true;
	}

	if (verts.Size() == 4)
	{
		VariantVector faces;
		faces.Resize(6);
		faces[0] = 0;
		faces[1] = 1;
		faces[2] = 2;
		faces[3] = 0;
		faces[4] = 3;
		faces[5] = 2;

		meshOut = TriMesh_Make(verts, faces);
		return true;
	}


	

	try {
		//submit this poly to poly2tri
		p2t::CDT cdt(p2tPoints);


		//add the holes
		for (unsigned i = 0; i < holes.Size(); i++)
		{
			VariantMap holeMap = holes[i].GetVariantMap();
			if (!holeMap.Keys().Contains("vertices"))
				continue;

			VariantVector holeVerts = holeMap["vertices"].GetVariantVector();
			int numHVerts = holeVerts.Size();

			//check if closed
			float endPointDistance = (holeVerts[0].GetVector3() - holeVerts[numHVerts - 1].GetVector3()).Length();
			bool isClosed = (endPointDistance < 1.0f / Urho3D::M_LARGE_VALUE) ? true : false;

			//exit if not closed
			if (!isClosed)
				continue;

			//otherwise proceed
			std::vector<p2t::Point*> holePts;
			GetP2TPointsFromVariant(holes[i], Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0, 0.0), holePts, true);
			AddPointsToMap(ptIndexMap, holePts);
			cdt.AddHole(holePts);

			//erase the duplicate at end
			holeVerts.Erase(numHVerts - 1, 1);
			numHVerts = holeVerts.Size();

			//also append these points to the vertex list
			//make sure to reverse'
			for (int j = 0; j < numHVerts; j++)
			{
				verts.Push(holeVerts[numHVerts - j - 1]);
				//verts.Push(holeVerts[j]);
			}
		}

		//triangulate
		try
		{
			cdt.Triangulate();
			std::list<p2t::Triangle*> tMap = cdt.GetMap();

			//export the data
			std::vector< p2t::Triangle* > triangles = cdt.GetTriangles();
			VariantVector faces;
			for (int i = 0; i < triangles.size(); i++)
			{
				p2t::Triangle* curr_tri = triangles[i];

				//need to check winding of triangle coming from poly2tri.
				p2t::Point* pa = curr_tri->GetPoint(0);
				p2t::Point* pb = curr_tri->GetPoint(1);
				p2t::Point* pc = curr_tri->GetPoint(2);

				int a = ptIndexMap[pa];
				int b = ptIndexMap[pb];
				int c = ptIndexMap[pc];

				faces.Push(a); faces.Push(c); faces.Push(b);
			}

			meshOut = TriMesh_Make(verts, faces);
		}
		catch (const std::runtime_error& error)
		{
			return false;
		}
	}
	catch (const std::runtime_error& error)
	{
		return false;
	}

	return true;
}

bool Geomlib::TriangulateFace(const Urho3D::Variant & NMeshIn, int faceIndex, Urho3D::VariantVector& tri_face_list)
{
	//data check
	bool dataCheck = false;
	if (NMeshIn.GetType() == VariantType::VAR_VARIANTMAP)
	{
		if (NMeshIn.GetVariantMap().Keys().Contains("vertices"))
			dataCheck = true;
	}

	if (!dataCheck)
	{
		return false;
	}

	// we will use the face_indices (ints of the face verts to map the new faces back to old indices)
	VariantVector face_indices;
	Urho3D::Variant facePoly = NMesh_GetFacePolyline(NMeshIn, faceIndex, face_indices);

	//get the data
	VariantMap polyMap = facePoly.GetVariantMap();

	VariantVector verts = Polyline_ComputeSequentialVertexList(facePoly);
	int numVerts = verts.Size();

	//early exit for tris and quads

	if (verts.Size() == 3)
	{
		tri_face_list.Push(Variant(face_indices[0].GetInt()));
		tri_face_list.Push(Variant(face_indices[1].GetInt()));
		tri_face_list.Push(Variant(face_indices[2].GetInt()));

		return true;
	}

	if (verts.Size() == 4)
	{
		tri_face_list.Push(face_indices[0].GetInt());
		tri_face_list.Push(face_indices[1].GetInt());
		tri_face_list.Push(face_indices[2].GetInt());
		tri_face_list.Push(face_indices[0].GetInt());
		tri_face_list.Push(face_indices[2].GetInt());
		tri_face_list.Push(face_indices[3].GetInt());

		return true;
	}


	Vector3 lP, lN;
	Geomlib::BestFitPlane(verts, lP, lN);
	int max_index = LargestMagnitudeIndex(lN);

	numVerts = verts.Size();

	//TODO::find the best fit transform
	Matrix3x4 bestFitFrame = Matrix3x4::IDENTITY;
	Vector3 localUp = bestFitFrame * Vector3::UP;

	//import the points to poly 
	std::vector<std::vector<double>> poly;
	std::vector<p2t::Point*> plane_pts;
	std::map<p2t::Point*, int> ptIndexMap;

	for (int i = 0; i < numVerts; i++)
	{
		Vector3 currVert = verts[i].GetVector3();
		Vector3 fitVert = bestFitFrame.Inverse() * currVert;
		float coords[3] = { currVert.x_, currVert.y_, currVert.z_ };

		int i0 = (max_index == 0) ? 1 : 0;
		int i1 = (max_index == 2) ? 1 : 2;

		//add as planar pair
		//p2t::Point* p = new p2t::Point(fitVert.x_, fitVert.z_);
		p2t::Point* p = new p2t::Point(coords[i0], coords[i1]);
		plane_pts.push_back(p);
		ptIndexMap[p] = i;
	}

	//add this contour to the poly
	//poly.push_back(plane_pts);

	//submit this poly to poly2tri
	p2t::CDT cdt(plane_pts);

	//triangulate
	cdt.Triangulate();
	std::list<p2t::Triangle*> tMap = cdt.GetMap();

	//export the data
	std::vector< p2t::Triangle* > triangles = cdt.GetTriangles();
	VariantVector faces;
	for (int i = 0; i < triangles.size(); i++)
	{
		p2t::Triangle* curr_tri = triangles[i];

		//need to check winding of triangle coming from poly2tri.
		p2t::Point* pa = curr_tri->GetPoint(0);
		p2t::Point* pb = curr_tri->GetPoint(1);
		p2t::Point* pc = curr_tri->GetPoint(2);

		int a = ptIndexMap[pa];
		int b = ptIndexMap[pb];
		int c = ptIndexMap[pc];

		faces.Push(a); faces.Push(c); faces.Push(b);
        //faces.Push(a); faces.Push(b); faces.Push(c);
	}

	// remap to global vertex indices
	for (int i = 0; i < faces.Size(); ++i) {
		tri_face_list.Push(face_indices[faces[i].GetInt()]);
	}

	return true;
}

bool Geomlib::TriangulatePolygon(const Urho3D::Variant& polyIn, Urho3D::Variant& meshOut)
{
	//data check
	bool dataCheck = false;
	if (polyIn.GetType() == VariantType::VAR_VARIANTMAP)
	{
		if (polyIn.GetVariantMap().Keys().Contains("vertices"))
			dataCheck = true;
	}

	if (!dataCheck)
	{
		return false;
	}

	//get the data
	VariantMap polyMap = polyIn.GetVariantMap();
	VariantVector verts = Polyline_ComputeSequentialVertexList(polyIn);
	int numVerts = verts.Size();

	//early exit for tris and quads

	if (verts.Size() == 3)
	{
		VariantVector faces;
		faces.Resize(3);
		faces[0] = 0;
		faces[1] = 2;
		faces[2] = 1;

		meshOut = TriMesh_Make(verts, faces);
		return true;
	}

	if (verts.Size() == 4)
	{
		VariantVector faces;
		faces.Resize(6);
		faces[0] = 0;
		faces[1] = 2;
		faces[2] = 1;
		faces[3] = 0;
		faces[4] = 2;
		faces[5] = 3;

		meshOut = TriMesh_Make(verts, faces);
		return true;
	}


	Vector3 lP, lN;
	Geomlib::BestFitPlane(verts, lP, lN);
	int max_index = LargestMagnitudeIndex(lN);

	//check if closed
	float endPointDistance = (verts[0].GetVector3() - verts[numVerts - 1].GetVector3()).Length();
	bool isClosed = (endPointDistance < 1.0f / Urho3D::M_LARGE_VALUE) ? true : false;

	//exit if not closed
	if (!isClosed)
		return false;

	//erase the duplicate at end
	verts.Erase(numVerts - 1, 1);
	numVerts = verts.Size();

	//TODO::find the best fit transform
	Matrix3x4 bestFitFrame = Matrix3x4::IDENTITY;
	Vector3 localUp = bestFitFrame * Vector3::UP;

	//import the points to poly 
	std::vector<std::vector<double>> poly;
	std::vector<p2t::Point*> plane_pts;
	std::map<p2t::Point*, int> ptIndexMap;

	for (int i = 0; i < numVerts; i++)
	{
		Vector3 currVert = verts[i].GetVector3();
		Vector3 fitVert = bestFitFrame.Inverse() * currVert;
		float coords[3] = { currVert.x_, currVert.y_, currVert.z_ };

		int i0 = (max_index == 0) ? 1 : 0;
		int i1 = (max_index == 2) ? 1 : 2;

		//add as planar pair
		//p2t::Point* p = new p2t::Point(fitVert.x_, fitVert.z_);
		p2t::Point* p = new p2t::Point(coords[i0], coords[i1]);
		plane_pts.push_back(p);
		ptIndexMap[p] = i;
	}

	//add this contour to the poly
	//poly.push_back(plane_pts);

	//submit this poly to poly2tri
	p2t::CDT cdt(plane_pts);

	//triangulate
	cdt.Triangulate();
	std::list<p2t::Triangle*> tMap = cdt.GetMap();

	//export the data
	std::vector< p2t::Triangle* > triangles = cdt.GetTriangles();
	VariantVector faces;
	for (int i = 0; i < triangles.size(); i++)
	{
		p2t::Triangle* curr_tri = triangles[i];

		//need to check winding of triangle coming from poly2tri.
		p2t::Point* pa = curr_tri->GetPoint(0);
		p2t::Point* pb = curr_tri->GetPoint(1);
		p2t::Point* pc = curr_tri->GetPoint(2);

		int a = ptIndexMap[pa];
		int b = ptIndexMap[pb];
		int c = ptIndexMap[pc];

		faces.Push(a); faces.Push(c); faces.Push(b);
	}

	meshOut = TriMesh_Make(verts, faces);
	return true;
}
