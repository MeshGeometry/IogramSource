#include "Geomlib_PolylinePointFromParameter.h"

#include "Polyline.h"

// Inputs:
//   polyline: storing type VAR_VARIANTMAP, with map storing type VAR_VARIANTVECTOR under key "vertices", with those variants storing type VAR_VECTOR3;
//             >= 2 distinct points in the polyline
//   t:        0 <= t <= 1 (t will be set to an appropriate endpoint if it's outside the valid interval
// Outputs:
//   point: 3D point on the line corresponding to parameter t; start of polyline is t=0, end of polyline is t=1
// Returns true if computation was successfully carried out.
bool Geomlib::PolylinePointFromParameter(
	const Urho3D::Variant& polyline,
	float t,
	Urho3D::Vector3& point
)
{
	using Urho3D::VariantVector;
	using Urho3D::Vector3;

	//////////////////////////////////
	// Extract and validate input data
	//////////////////////////////////

	if (!Polyline_Verify(polyline)) {
		return false;
	}
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	const VariantVector vertexList = Polyline_ComputeSequentialVertexList(polyline);
	if (vertexList.Size() == 0) {
		return false;
	}
	else if (vertexList.Size() == 1) {
		// degenerate 1-point polyline
		point = vertexList[0].GetVector3();
		return true;
	}

	////////////////////////////////////////////////////////
	// Vertex count is >= 2, proper computation begins here.
	////////////////////////////////////////////////////////

	float totalLength = 0.0f;
	// loop over each edge [i, i + 1]
	for (unsigned i = 0; i < vertexList.Size() - 1; ++i) {
		Vector3 seg = vertexList[i + 1].GetVector3() - vertexList[i].GetVector3();
		totalLength += seg.Length();
	}

	if (Urho3D::Equals(totalLength, 0.0f)) {
		// All points in polyline are identical, or, more likely,
		// the Variants do not store Vector3's and so return the origin for GetVector3().
		point = vertexList[0].GetVector3();
		return true;
	}

	float targetDistance = t * totalLength;

	// loop over each edge [i, i + 1]
	float startLength = 0.0f;
	for (unsigned i = 0; i < vertexList.Size() - 1; ++i) {
		Vector3 start = vertexList[i].GetVector3();
		Vector3 end = vertexList[i + 1].GetVector3();
		Vector3 seg = end - start;
		float segLength = seg.Length();
		float endLength = startLength + segLength;

		// If point lies on this segment....
		if (endLength >= targetDistance) {
			if (Urho3D::Equals(segLength, 0.0f)) {
				point = start; // or end, since in this case start == end
				return true;
			}
			float s = (targetDistance - startLength) / segLength;
			point = start + s * seg;
			return true;
		}

		// Point lies on a later segment. Update and carry on.
		startLength = endLength;
	}

	// If computation logic is correct, we should never arrive here.
	return false;
}