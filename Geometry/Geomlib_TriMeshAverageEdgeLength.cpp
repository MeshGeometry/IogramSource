#include "Geomlib_TriMeshAverageEdgeLength.h"

#include <Eigen/Core>

#pragma warning(push, 0)
#include <igl/avg_edge_length.h>
#pragma warning(pop)

#include "ConversionUtilities.h"
#include "TriMesh.h"

float Geomlib::TriMeshAverageEdgeLength(const Urho3D::Variant& tri_mesh)
{
	if (!TriMesh_Verify(tri_mesh)) {
		return 0.0f;
	}

	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	IglMeshToMatrices(tri_mesh, V, F);

	return (float)igl::avg_edge_length(V, F);
}