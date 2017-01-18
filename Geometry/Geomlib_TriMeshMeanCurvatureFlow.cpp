#include "Geomlib_TriMeshMeanCurvatureFlow.h"

#pragma warning(push, 0)
#include <igl/is_vertex_manifold.h>
#include <igl/is_edge_manifold.h>
#include <igl/cotmatrix.h>
#include <igl/massmatrix.h>
#include <igl/doublearea.h>
#include <igl/barycenter.h>
#pragma warning(pop)

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "TriMesh.h"
#include "ConversionUtilities.h"

namespace {

bool IglMeanCurvatureStep(
	const Eigen::MatrixXf& V,
	const Eigen::MatrixXi& F,
	Eigen::MatrixXf& NV,
	Eigen::MatrixXi& NF
)
{
	Eigen::MatrixXf U = V;

	Eigen::SparseMatrix<float> L;
	igl::cotmatrix(V, F, L);

	Eigen::SparseMatrix<float> M;
	igl::massmatrix(U, F, igl::MASSMATRIX_TYPE_BARYCENTRIC, M);

	const auto & S = (M - 0.001f * L);
	Eigen::SimplicialLLT<Eigen::SparseMatrix<float> > solver(S);
	if (solver.info() != Eigen::Success) {
		return false;
	}
	U = solver.solve(M * U).eval();

	// Compute centroid and subtract (also important for numerics)
	Eigen::VectorXf doubleArea;
	igl::doublearea(U, F, doubleArea);
	float area = 0.5f * doubleArea.sum();
	if (!(area > 0.0f)) { // we divide by it later
		return false;
	}

	Eigen::MatrixXf BC;
	igl::barycenter(U, F, doubleArea);
	Eigen::RowVector3f centroid(0, 0, 0);
	for (int i = 0; i < BC.rows(); ++i) {
		centroid += (0.5f * doubleArea(i) / area) * BC.row(i);
	}
	U.rowwise() -= centroid;

	// Normalize to unit surface area (important for numerics)
	if (!(sqrt(area) > 0.0f)) { // do you get 0.0f if you sqrt a really small float?
		return false;
	}
	U.array() /= sqrt(area);

	NV = U;
	NF = F;

	return true;
}

} //

using Urho3D::Variant;

Urho3D::Variant Geomlib::TriMesh_MeanCurvatureFlowStep(
	const Urho3D::Variant& tri_mesh
)
{
	if (!TriMesh_Verify(tri_mesh)) {
		return Variant();
	}

	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	IglMeshToMatrices(tri_mesh, V, F);

	/*
	if (!igl::is_edge_manifold(V, F)) {
		return Variant();
	}
	Eigen::VectorXi B;
	if (!igl::is_vertex_manifold(F, B)) {
		return Variant();
	}
	*/

	// V, F are ok
	Eigen::MatrixXf NV;
	Eigen::MatrixXi NF;
	bool success = IglMeanCurvatureStep(V, F, NV, NF);
	if (!success) {
		return Variant();
	}

	return TriMesh_Make(NV, NF);
}

bool Geomlib::TriMesh_MeanCurvatureFlow(
	const Urho3D::Variant& tri_mesh,
	int num_steps,
	Urho3D::Variant& tri_mesh_out
)
{
	if (!TriMesh_Verify(tri_mesh)) {
		return false;
	}
	if (num_steps <= 0) {
		return false;
	}

	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	IglMeshToMatrices(tri_mesh, V, F);

	// V, F are ok here

	for (int i = 0; i < num_steps; ++i) {

		Eigen::MatrixXf VV;
		Eigen::MatrixXi FF;
		bool success = IglMeanCurvatureStep(V, F, VV, FF);
		if (!success) {
			return false;
		}
		V.setZero(VV.rows(), VV.cols());
		V = VV;
		F.setZero(FF.rows(), FF.cols());
		F = FF;
	}

	tri_mesh_out = TriMesh_Make(V, F);
	if (!TriMesh_Verify(tri_mesh_out)) {
		return false;
	}
	return true;
}