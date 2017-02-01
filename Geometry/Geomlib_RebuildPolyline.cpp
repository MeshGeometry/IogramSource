#include "Geomlib_RebuildPolyline.h"
#include "Geomlib_PolylinePointFromParameter.h"
#include "Polyline.h"

using namespace Urho3D;

Urho3D::Variant Geomlib::RebuildPolyline(
	const Urho3D::Variant& polylineIn,
	int numSegments
)
{
	bool ver = Polyline_Verify(polylineIn);
	if (!ver)
	{
		return false;
	}

	bool closed = Polyline_IsClosed(polylineIn);
	VariantVector newVerts;

	//push the first point
	Vector3 pt;
	for (int i = 0; i <= numSegments; i++)
	{
		float t = (float)i / (float)numSegments;
		PolylinePointFromParameter(polylineIn, t, pt);
		newVerts.Push(pt);
	}

	return Polyline_Make(newVerts);
}