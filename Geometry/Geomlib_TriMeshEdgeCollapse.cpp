#include "Geomlib_TriMeshEdgeCollapse.h"

#include <vector>

#pragma warning(push, 0)
#include <igl/remove_unreferenced.h>
#include <igl/edge_flaps.h>
#include <igl/collapse_edge.h>
#include <igl/is_edge_manifold.h>
#include <igl/is_vertex_manifold.h>
#pragma warning(pop)

#include <Eigen/Core>

#include "TriMesh.h"
#include "ConversionUtilities.h"

using Urho3D::Variant;

namespace {

void collapse_edges_below_length(
	const Eigen::MatrixXd& OV,
	const Eigen::MatrixXi& OF,
	Eigen::MatrixXd& NV,
	Eigen::MatrixXi& NF,
	double L
)
{
	Eigen::MatrixXd V = OV;
	Eigen::MatrixXi F = OF;

	// Prepare array-based edge data structures
	Eigen::VectorXi EMAP;
	Eigen::MatrixXi E, EF, EI;
	igl::edge_flaps(F, E, EMAP, EF, EI);

	Eigen::RowVector3d mid;
	double len;
	bool b;

	int numc = -1;
	while (numc != 0) {
		numc = 0;
		for (int e = 0; e < E.rows(); ++e) {
			len = (V.row(E(e, 0)) - V.row(E(e, 1))).norm();
			if (len < L) {
				mid = 0.5 * (V.row(E(e, 0)) + V.row(E(e, 1)));
				b = igl::collapse_edge(e, mid, V, F, E, EMAP, EF, EI);
				if (b) {
					++numc;
				}
			}
		}
	}

	Eigen::VectorXi _1;
	igl::remove_unreferenced(V, F, NV, NF, _1);

	std::vector<int> facePatch;
	for (int i = 0; i < NF.rows(); ++i) {
		if ((NF(i, 0) == 0) && (NF(i, 1) == 0) && (NF(i, 2) == 0)) {
		}
		else {
			facePatch.push_back(NF(i, 0));
			facePatch.push_back(NF(i, 1));
			facePatch.push_back(NF(i, 2));
		}
	}
	Eigen::MatrixXi NF2(facePatch.size() / 3, 3);
	for (int i = 0; i < facePatch.size() / 3; ++i) {
		NF2(i, 0) = facePatch[3 * i];
		NF2(i, 1) = facePatch[3 * i + 1];
		NF2(i, 2) = facePatch[3 * i + 2];
	}
	NF.setZero(NF2.rows(), 3);
	NF = NF2;
}

} // namespace

Urho3D::Variant Geomlib::TriMesh_CollapseShortEdges(
	const Urho3D::Variant& tri_mesh,
	float collapse_threshold
)
{
	if (!TriMesh_Verify(tri_mesh)) {
		return Variant();
	}

	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	IglMeshToMatrices(tri_mesh, V, F);

	if (!igl::is_edge_manifold(V, F)) {
		return Variant();
	}

	Eigen::VectorXi B;
	if (!igl::is_vertex_manifold(F, B)) {
		return Variant();
	}

	// Convert from float to double
	Eigen::MatrixXd V_d = IglFloatToDouble(V);


	Eigen::MatrixXd NV_d;
	Eigen::MatrixXi NF;
	collapse_edges_below_length(V_d, F, NV_d, NF, collapse_threshold);

	Eigen::MatrixXf NV = IglDoubleToFloat(NV_d);

	return TriMesh_Make(NV, NF);
}