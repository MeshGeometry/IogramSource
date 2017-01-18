#include "Geomlib_HausdorffDistance.h"

#pragma warning(push, 0)
#include <igl/hausdorff.h>
#pragma warning(pop)

#include "ConversionUtilities.h"
#include "TriMesh.h"

bool Geomlib::TriMesh_HausdorffDistance(
	const Urho3D::Variant& mesh1,
	const Urho3D::Variant& mesh2,
	float& distance
)
{
	if (!TriMesh_Verify(mesh1) || !TriMesh_Verify(mesh2)) {
		return false;
	}

	Eigen::MatrixXf V1, V2;
	Eigen::MatrixXi F1, F2;
	IglMeshToMatrices(mesh1, V1, F1);
	IglMeshToMatrices(mesh2, V2, F2);

	float d = -1.0f;
	igl::hausdorff(V1, F1, V2, F2, d);
	if (d >= 0.0f) {
		distance = d;
		return true;
	}
	else {
		return false;
	}
}