#include "Geomlib_TriMeshLoopSubdivide.h"

#include <assert.h>

#include <iostream>
#include <vector>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Vector3.h>

#include <Eigen/Core>

#pragma warning(push, 0)
#include <igl/edges.h>
#include <igl/edge_flaps.h>
#include <igl/is_boundary_edge.h>
#include <igl/is_border_vertex.h>
#include <igl/adjacency_list.h>
#include <igl/boundary_loop.h>
#pragma warning(pop)

#include "ConversionUtilities.h"
#include "TriMesh.h"

#pragma warning(disable : 4244)

// helper functions quarantined in anonymous namespace
namespace {

	void loop_subdivide_mesh(
		const Eigen::MatrixXf& V,
		const Eigen::MatrixXi& F,
		Eigen::MatrixXf& NV,
		Eigen::MatrixXi& NF
	)
	{
		unsigned numVertices = V.rows();

		// precompute the number of edges
		Eigen::MatrixXi EE;
		//igl::all_edges(F, EE);
		igl::edges(F, EE);

		// precompute edge-face relationship data
		Eigen::MatrixXi E, EI;
		Eigen::MatrixXi EF = Eigen::MatrixXi::Constant(EE.rows(), 2, -1);
		Eigen::VectorXi EMAP;
		igl::edge_flaps(F, E, EMAP, EF, EI);

		// precompute edge boundary data
		Eigen::VectorXi B;
		igl::is_boundary_edge(E, F, B);

		// precompute vertex boundary data
		std::vector<bool> borderFlags = igl::is_border_vertex(V, F);
		std::vector<std::vector<int> > boundaryLoops;
		igl::boundary_loop(F, boundaryLoops);

		// precompute adjacency data
		std::vector<std::vector<int> > adjacencyData;
		igl::adjacency_list(F, adjacencyData);

		//
		NV.resize(V.rows() + E.rows(), 3);
		NV.block(0, 0, numVertices, 3) = V;

		for (unsigned i = 0; i < boundaryLoops.size(); ++i) {

			std::vector<int> loop = boundaryLoops[i];

			for (unsigned j = 0; j < loop.size(); ++j) {
				unsigned prev = (j == 0) ? (loop.size() - 1) : (j - 1);
				unsigned next = (j == loop.size() - 1) ? 0 : (j + 1);
				NV.row(loop[j]) = 0.125f * V.row(loop[prev]) + 0.75f * V.row(loop[j]) + 0.125f * V.row(loop[next]);
			}

		}

		for (unsigned i = 0; i < V.rows(); ++i) {
			if (borderFlags[i]) {
				// border vertex that should have been accounted for at the boundary loop step
			}
			else {
				// interior vertex
				std::vector<int> adj = adjacencyData[i];
				unsigned n = adj.size();
				if (n < 3) {
					std::cerr << "WARNING: loop_subdivide_mesh --- skipping bad vertex....\n";
				}
				else {
					float beta = (n == 3) ? 0.1875f : (0.375f * (1.0f / n));
					Eigen::RowVector3f w = (1.0f - n * beta) * V.row(i);
					for (unsigned j = 0; j < adj.size(); ++j) {
						w += beta * V.row(adj[j]);
					}
					NV.row(i) = w;
				}
			}
		}

		// add points on each edge
		for (unsigned e = 0; e < E.rows(); ++e) {

			int v0 = E(e, 0);
			int v1 = E(e, 1);

			if (B(e) == 0) {
				int v2 = F(EF(e, 0), EI(e, 0));
				int v3 = F(EF(e, 1), EI(e, 1));

				NV.row(numVertices + e) = 0.375f * V.row(v0) + 0.375f * V.row(v1) + 0.125f * V.row(v2) + 0.125f * V.row(v3);
			}
			else if (B(e) == 1) {
				NV.row(numVertices + e) = 0.5f * V.row(v0) + 0.5f * V.row(v1);
			}
			else {
				//
				std::cerr << "WARNING: loop_subdivide_mesh --- skipping edge\n";
			}
		}

		// Subdivide face data
		NF.resize(4 * F.rows(), 3);
		for (unsigned f = 0; f < F.rows(); ++f) {
			// if this is wrong, tho, how on earth are the faces right?
			// ?? anyway, alternative is 3*f + 0,1,2
			int v0 = V.rows() + EMAP(f);
			int v1 = V.rows() + EMAP(F.rows() + f);
			int v2 = V.rows() + EMAP(2 * F.rows() + f);
			/*
			int v0 = V.rows() + EMAP(3 * f);
			int v1 = V.rows() + EMAP(3 * f + 1);
			int v2 = V.rows() + EMAP(3 * f + 2);
			*/

			NF.row(4 * f) = Eigen::RowVector3i(v0, v1, v2);
			NF.row(4 * f + 1) = Eigen::RowVector3i(F(f, 0), v2, v1);
			NF.row(4 * f + 2) = Eigen::RowVector3i(v2, F(f, 1), v0);
			NF.row(4 * f + 3) = Eigen::RowVector3i(v1, v0, F(f, 2));
		}
	}
}

bool Geomlib::TriMeshLoopSubdivide(
	const Urho3D::Variant& meshIn,
	int steps,
	Urho3D::Variant& meshOut
)
{
	if (!TriMesh_Verify(meshIn)) {
		meshOut = Urho3D::Variant();
		return false;
	}

	Eigen::MatrixXf V, NV;
	Eigen::MatrixXi F, NF;
	IglMeshToMatrices(meshIn, V, F);

	loop_subdivide_mesh(V, F, NV, NF);

	meshOut = TriMesh_Make(NV, NF);
	return true;
}