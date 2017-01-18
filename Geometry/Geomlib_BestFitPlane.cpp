#include "Geomlib_BestFitPlane.h"

#include <assert.h>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/SVD>

namespace {

Eigen::Vector3f ComputeCentroid(const Eigen::MatrixXf& V)
{
	assert(V.rows() == 3);

	Eigen::Vector3f c(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < V.cols(); ++i) {
		c += V.col(i);
	}

	if (V.cols() != 0) {
		float sc = (1.0f / V.cols());
		c *= sc;
	}

	return c;
}


} // namespace

void Geomlib::BestFitPlane(
	const Urho3D::Vector<Urho3D::Vector3>& point_cloud,
	Urho3D::Vector3& point,
	Urho3D::Vector3& normal
)
{
	using Urho3D::Vector3;

	int num_vertices = (int)point_cloud.Size();
	if (num_vertices <= 0) {
		return;
	}

	Eigen::MatrixXf V(3, num_vertices);
	for (int i = 0; i < num_vertices; ++i) {
		Vector3 vec = point_cloud[i];
		V.col(i) = Eigen::Vector3f(vec.x_, vec.y_, vec.z_);
	}

	Eigen::Vector3f c = ComputeCentroid(V);

	Eigen::MatrixXf NV = V;
	for (int i = 0; i < NV.cols(); ++i) {
		NV.col(i) -= c;
	}

	Eigen::JacobiSVD<Eigen::MatrixXf> svd(NV, Eigen::DecompositionOptions::ComputeThinU);

	Eigen::MatrixXf U = svd.matrixU();
	assert(U.rows() == 3 && U.cols() == 3);

	normal = Vector3(U(0, 2), U(1, 2), U(2, 2));
	point = Vector3(c(0), c(1), c(2));
}

void Geomlib::BestFitPlane(
	const Urho3D::VariantVector& vertex_list,
	Urho3D::Vector3& point,
	Urho3D::Vector3& normal
)
{
	using Urho3D::Vector;
	using Urho3D::Vector3;

	Vector<Vector3> point_cloud;
	for (int i = 0; i < vertex_list.Size(); ++i) {
		point_cloud.Push(vertex_list[i].GetVector3());
	}

	BestFitPlane(point_cloud, point, normal);
}
