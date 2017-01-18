#pragma once

#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Core/Variant.h>

namespace Geomlib {
	/*
	Triangulates a closed polyline.
	If the polyline is non planar, a best fit plane will be found and the triangulation attempted.
	In this case, it is unlikely that a good triangulation will result.
	*/

	bool TriangulatePolygon(const Urho3D::Variant& polyIn, Urho3D::Variant& polyOut);
	bool TriangulatePolygon(const Urho3D::Variant& polyIn, 
							const Urho3D::VariantVector& holes, 
							Urho3D::Variant& polyOut);

	// Some methods for NMesh --> TriMesh conversion
	// Triangulates a specified face of an NMesh. 
	bool TriangulateFace(const Urho3D::Variant& NMeshIn,
		int faceIndex,
		Urho3D::VariantVector& tri_face_list
		);
};