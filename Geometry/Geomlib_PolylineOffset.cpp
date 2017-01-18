#include "Geomlib_PolylineOffset.h"
#include "Polyline.h"

using Urho3D::Variant;
using Urho3D::VariantVector;
using Urho3D::Vector3;

bool Geomlib::PolylineOffset(
	const Urho3D::Variant& polyIn,
	Urho3D::Variant& polyOut,
	float offset
	)
{
	if (!Polyline_Verify(polyIn))
	{
		return false;
	}

	const VariantVector verts = Polyline_ComputeSequentialVertexList(polyIn);

	VariantVector offsetVerts;

	// Check if polyline is closed
	bool isClosed = false;
	if (verts[0].GetVector3() == verts[verts.Size() - 1].GetVector3())
		isClosed = true;

	// we'll treat the endpoints using the offset vectors for the penultimate vertices on either end
	Vector3 startVec;
	Vector3 endVec;
	// for now, push a placeholder for the first vertex on to the vector
	offsetVerts.Push(Vector3::ZERO);
	int vertCount = verts.Size();
	for (int i = 1; i < vertCount - 1; ++i)
	{
		// the vertex to offset
		Vector3 curVert = verts[i].GetVector3();

		// get the vectors defining the segments adjacent to the current vert
		Vector3 vecA = curVert - verts[i - 1].GetVector3();
		Vector3 vecB = curVert - verts[i + 1].GetVector3();

		// find the bisecting vector and normalize for offset direction vector
		Vector3 offsetVec = vecA + vecB;
		// save the vectors for the start and end verts
		offsetVec.Normalize();
		if (i == 1)
			startVec = offsetVec;
		if (i == (vertCount - 2))
			endVec = offsetVec;

		// actually offset the current vertex and save to new vertex list
		Vector3 offVert = curVert + offset*offsetVec;
		offsetVerts.Push(offVert);
	}

	// Now treat the end points, depending on whether the polyline is closed or not.
	if (isClosed) {
		Vector3 averageVec = startVec + endVec;
		averageVec.Normalize();
		Vector3 curVert = verts[0].GetVector3();
		Vector3 endOffsetVert = curVert + offset*averageVec;
		offsetVerts[0] = endOffsetVert; //start vert
		offsetVerts.Push(endOffsetVert); //end vert
	}
	else {
		// start vert
		Vector3 startOffsetVert = verts[0].GetVector3() + offset*startVec;
		offsetVerts[0] = startOffsetVert;

		//end vert
		Vector3 endOffsetVert = verts[vertCount - 1].GetVector3();
		offsetVerts.Push(endOffsetVert + offset*endVec);
	}

	polyOut = Polyline_Make(offsetVerts);
	return true;
}