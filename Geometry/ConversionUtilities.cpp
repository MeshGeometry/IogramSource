//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "ConversionUtilities.h"

#include <assert.h>

#include <iostream>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Dense>

#pragma warning(push, 0)
#include <igl/vertex_triangle_adjacency.h>
#pragma warning(pop)

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>

#pragma warning(disable : 4244)

using Urho3D::Variant;
using Urho3D::VariantMap;
using Urho3D::VariantType;
using Urho3D::VariantVector;
using Urho3D::Vector3;

// Extract data stored in IGL matrices V, F into VariantVector

VariantVector IglExtractVertices(
	const Eigen::MatrixXf& V
)
{
	VariantVector earlyRet;

	if (V.rows() == 0) {
		std::cout << "ERROR: IglExtractVertices --- V.rows() == 0" << std::endl;
		return earlyRet;
	}
	if (V.cols() != 3) {
		int vcols = V.cols();
		std::cout << "ERROR: IglExtractVertices --- V.cols() != 3" << std::endl;
		return earlyRet;
	}

	VariantVector vertices;

	for (unsigned i = 0; i < V.rows(); ++i) {
		Vector3 vert(V(i, 0), V(i, 1), V(i, 2));
		vertices.Push(Variant(vert));
	}

	return vertices;
}

VariantVector IglExtractFaces(
	const Eigen::MatrixXi& F
)
{
	VariantVector faces;

	for (unsigned i = 0; i < F.rows(); ++i) {
		faces.Push(Variant(F(i, 0)));
		faces.Push(Variant(F(i, 1)));
		faces.Push(Variant(F(i, 2)));
	}

	return faces;
}

VariantVector IglExtractFaces(
	const Eigen::MatrixXi& F,
	unsigned numVertices
)
{
	VariantVector earlyRet;

	if (F.rows() == 0) {
		std::cout << "ERROR: IglExtractFaces --- F.rows() == 0" << std::endl;
		return earlyRet;
	}

	if (F.cols() != 3) {
		std::cout << "ERROR: IglExtractFaces --- F.rows() != 3" << std::endl;
		return earlyRet;
	}

	VariantVector faces;

	for (unsigned i = 0; i < F.rows(); ++i) {
		for (unsigned j = 0; j < 3; ++j) {
			int vertIndex = F(i, j);
			if (vertIndex < 0 || vertIndex > numVertices - 1) {
				std::cout << "ERROR: IglExtractFaces --- vertIndex out of range" << std::endl;
				return earlyRet;
			}
			faces.Push(Variant(F(i, j)));
		}
	}

	return faces;
}

Eigen::MatrixXd IglFloatToDouble(
	const Eigen::MatrixXf& V
)
{
	Eigen::MatrixXd Vd(V.rows(), V.cols());
	for (unsigned i = 0; i < V.rows(); ++i) {
		Vd(i, 0) = double(V(i, 0));
		Vd(i, 1) = double(V(i, 1));
		Vd(i, 2) = double(V(i, 2));
	}

	return Vd;
}

Eigen::MatrixXf IglDoubleToFloat(
	const Eigen::MatrixXd& V
)
{
	Eigen::MatrixXf Vf(V.rows(), V.cols());
	for (unsigned i = 0; i < V.rows(); ++i) {
		Vf(i, 0) = (float)V(i, 0);
		Vf(i, 1) = (float)V(i, 1);
		Vf(i, 2) = (float)V(i, 2);
	}

	return Vf;
}

VariantVector IglComputeFaceNormals(
	const Eigen::MatrixXf& V,
	const Eigen::MatrixXi& F
)
{
	VariantVector normals;

	for (unsigned i = 0; i < F.rows(); ++i) {
		Eigen::RowVector3f u = V.row(F(i, 1)) - V.row(F(i, 0));
		Eigen::RowVector3f v = V.row(F(i, 2)) - V.row(F(i, 0));

		//Eigen::RowVector3f n = u.cross(v);
		Eigen::RowVector3f n = u.cross(v);
		// does Urho expect this normalized?

		Vector3 normal(n(0), n(1), n(2));

		normals.Push(Variant(normal));
	}

	return normals;
}

VariantVector IglComputeVertexNormals(
	const Eigen::MatrixXf& V,
	const Eigen::MatrixXi& F
)
{
	VariantVector vertexNormals;
	VariantVector faceNormals = IglComputeFaceNormals(V, F);
	assert(faceNormals.Size() == F.rows());

	std::vector<std::vector<int> > VF;
	std::vector<std::vector<int> > VFi;

	igl::vertex_triangle_adjacency(V, F, VF, VFi);

	// for each vertex index i, compute the normal there
	// VF.size() == V.rows()
	for (unsigned i = 0; i < VF.size(); ++i) {

		// normal associated to vertex i
		Vector3 normal(0.0f, 0.0f, 0.0f);

		// normal will be the straight average of the normals of faces adjacent to vertex
		for (unsigned j = 0; j < VF[i].size(); ++j) {
			int adj = VF[i][j];
			normal = normal + faceNormals[adj].GetVector3();
		}
		if (VF[i].size() > 0) {
			normal = (1.0f / VF[i].size()) * normal;
		}
		vertexNormals.Push(normal);
	}

	return vertexNormals;
}

Urho3D::Variant IglMatricesToMesh(
	const Eigen::MatrixXf& V,
	const Eigen::MatrixXi& F
)
{
	Variant earlyRet;

	VariantVector outVertices;
	outVertices = IglExtractVertices(V);
	if (outVertices.Size() == 0) {
		std::cout << "ERROR: IglMatricesToMesh --- outVertices.Size() == 0" << std::endl;
		return earlyRet;
	}

	unsigned numVertices = V.rows();
	VariantVector outFaces = IglExtractFaces(F, numVertices);
	if (outFaces.Size() == 0) {
		std::cout << "ERROR: IglMatricesToMesh --- outFaces.Size() == 0" << std::endl;
		return earlyRet;
	}

	VariantMap meshMap;
	meshMap["vertices"] = outVertices;
	meshMap["faces"] = outFaces;

	Variant outMesh(meshMap);

	return outMesh;
}

bool IglMeshToMatrices(
	const Variant& mesh,
	Eigen::MatrixXf& V,
	Eigen::MatrixXi& F
)
{
	VariantMap meshMap = mesh.GetVariantMap();
	if (meshMap.Empty()) {
		// V, F untouched
		std::cout << "ERROR: IglMeshToMatrices --- mesh.GetVariantMap() returns empty" << std::endl;
		return false;
	}

	// meshMap is not empty
	Variant vertices = meshMap["vertices"];
	if (vertices.GetType() != VariantType::VAR_VARIANTVECTOR) {
		// V, F untouched
		std::cout << "ERROR: IglMeshToMatrices --- vertices.GetType() != VAR_VARIANTVECTOR" << std::endl;
		return false;
	}
	Variant faces = meshMap["faces"];
	if (faces.GetType() != VariantType::VAR_VARIANTVECTOR) {
		// V, F untouched
		std::cout << "ERROR: IglMeshToMatrices --- faces.GetType() != VAR_VARIANTVECTOR" << std::endl;
		return false;
	}

	VariantVector vertexList = vertices.GetVariantVector();
	if (vertexList.Size() == 0) {
		// V, F untouched
		std::cout << "ERROR: IglMeshToMatrices --- vertexList.Size() == 0" << std::endl;
		return false;
	}
	unsigned numVertices = vertexList.Size();
	Eigen::MatrixXf W(numVertices, 3);
	for (unsigned i = 0; i < numVertices; ++i) {
		Variant vertexVariant = vertexList[i];
		if (vertexVariant.GetType() != VariantType::VAR_VECTOR3) {
			// V, F untouched
			std::cout << "ERROR: IglMeshToMatrices --- vertexVariant.GetType() != VAR_VECTOR3, index=" << i << std::endl;
			return false;
		}
		Vector3 vert = vertexVariant.GetVector3();
		W.row(i) = Eigen::RowVector3f(vert.x_, vert.y_, vert.z_);
	}
	// vertex information is good; if face information checks out, W is ready to be copied to V

	VariantVector faceList = faces.GetVariantVector();
	if (faceList.Size() == 0) {
		// V, F untouched
		std::cout << "ERROR: IglMeshToMatrices --- faceList.Size() == 0" << std::endl;
		return false;
	}
	if (faceList.Size() % 3 != 0) {
		// V, F untouched
		std::cout << "ERROR: IglMeshToMatrices --- faceList.Size() % 3 != 0" << std::endl;
		return false;
	}
	unsigned numFaces = faceList.Size() / 3;
	Eigen::MatrixXi G(numFaces, 3);
	for (unsigned i = 0; i < numFaces; ++i) {
		for (unsigned j = 0; j < 3; ++j) {
			Variant faceIndexVar = faceList[3 * i + j];
			if (faceIndexVar.GetType() != VariantType::VAR_INT) {
				// V, F untouched
				std::cout << "ERROR: IglMeshMatrices -- faceIndexVar.GetType() != VAR_INT, index=" << 3 * i + j << std::endl;
				return false;
			}
			int faceIndex = faceIndexVar.GetInt();
			if (faceIndex < 0 || faceIndex > numVertices - 1) {
				// V, F untouched
				std::cout << "ERROR: IglMeshMatrices -- faceIndex out of range" << std::endl;
				return false;
			}
			G(i, j) = faceIndex;
		}
	}
	// G is ready

	/*
	Potential crash causing data NOT checked for:
	  -- face with repeated vertex, i.e., 0 0 0 or 0 1 1
	  -- bad mesh GEOMETRY, i.e., self-intersections, non-manifold
	*/
	V.setZero(W.rows(), 3);
	V = W;
	F.setZero(G.rows(), 3);
	F = G;

	return true;
}

// If data stored in mesh passes basic tests then this function essentially does
//   vertexList = mesh.GetVariantMap()["vertices"].GetVariantVector();
//   faceList = mesh.GetVariantMap()["faces"].GetVariantVector();
// and returns true;
// Otherwise Clears() vertexList and faceList and returns false.
// Basic tests are
//   (1) non-zero vertex count
//   (2) non-zero face count; consistent face count, i.e., list length divisible by 3
//   (3) all high level VariantType values are correct, that is,
//       mesh stores a VariantMap with the correct keys, etc.,
//       BUT there is NO check that
//         vertexList[0].GetType() == VAR_VECTOR3
//         faceList[0].GetType() == VAR_INT
//       etc.
bool ExtractMeshData(
	const Urho3D::Variant& mesh,
	Urho3D::VariantVector& vertexList,
	Urho3D::VariantVector& faceList
)
{
	// extract the map
	if (mesh.GetType() != VariantType::VAR_VARIANTMAP) {
		std::cerr << "ERROR: ExtractMeshData --- mesh.GetType() != VAR_VARIANTMAP\n";
		vertexList.Clear();
		faceList.Clear();
		return false;
	}
	VariantMap meshMap = mesh.GetVariantMap();

	// extract vertexList
	Variant vertices = meshMap["vertices"];
	if (vertices.GetType() != VariantType::VAR_VARIANTVECTOR) {
		std::cerr << "ERROR: ExtractMeshData --- vertices.GetType() != VAR_VARIANTVECTOR\n";
		vertexList.Clear();
		faceList.Clear();
		return false;
	}
	vertexList.Clear();
	vertexList = vertices.GetVariantVector();
	if (vertexList.Size() == 0) {
		std::cerr << "ERROR: ExtractMeshData --- copyVertexList.Size() == 0\n";
		vertexList.Clear();
		faceList.Clear();
		return false;
	}
	for (unsigned i = 0; i < vertexList.Size(); ++i) {
		if (vertexList[i].GetType() != VariantType::VAR_VECTOR3) {
			std::cerr << "ERROR: ExtractMeshData --- vertexList[i].GetType() != VAR_VECTOR3, i=" << i << "\n";
			vertexList.Clear();
			faceList.Clear();
			return false;
		}
	}

	// extract faceList
	Variant faces = meshMap["faces"];
	if (faces.GetType() != VariantType::VAR_VARIANTVECTOR) {
		std::cerr << "ERROR: ExtractMeshData --- faces.GetType() != VAR_VARIANTVECTOR\n";
		vertexList.Clear();
		faceList.Clear();
		return false;
	}
	faceList.Clear();
	faceList = faces.GetVariantVector();
	if (faceList.Size() == 0) {
		std::cerr << "ERROR: ExtractMeshData --- faceList.Size() == 0\n";
		vertexList.Clear();
		faceList.Clear();
		return false;
	}
	if (faceList.Size() % 3 != 0) {
		std::cerr << "ERROR: ExtractMeshData --- faceList.Size() % 3 != 0\n";
		vertexList.Clear();
		faceList.Clear();
		return false;
	}
	for (unsigned i = 0; i < faceList.Size(); ++i) {
		if (faceList[i].GetType() != VariantType::VAR_INT) {
			std::cerr << "ERROR: ExtractMeshData --- faceList[i].GetType() != VAR_INT, i=" << i << "\n";
			vertexList.Clear();
			faceList.Clear();
			return false;
		}
	}
	unsigned numVertices = vertexList.Size();
	for (unsigned i = 0; i < faceList.Size(); i += 3) {
		unsigned i0 = (unsigned)faceList[i].GetInt();
		unsigned i1 = (unsigned)faceList[i + 1].GetInt();
		unsigned i2 = (unsigned)faceList[i + 2].GetInt();

		if (i0 == i1 || i1 == i2 || i2 == i0) {
			std::cerr << "ERROR: ExtractMeshData --- repeated vertex indices in face\n";
			vertexList.Clear();
			faceList.Clear();
			return false;
		}

		if (
			(i0 < 0 || i0 > numVertices - 1) ||
			(i1 < 0 || i1 > numVertices - 1) ||
			(i2 < 0 || i2 > numVertices - 1)
			)
		{
			std::cerr << "ERROR: ExtractMeshData --- vertex indices out of range\n";
			vertexList.Clear();
			faceList.Clear();
			return false;
		}
	}

	return true;
}

void Matrix3x4ToEigen(const Urho3D::Matrix3x4& transform, Eigen::MatrixXf& rotation, Eigen::RowVector3f& translation)
{
	Eigen::MatrixXf eigenRotation;
	Eigen::RowVector3f eigenTranslation; 

	eigenRotation.setZero(3, 3);

	eigenRotation(0, 0) = transform.m00_;
	eigenRotation(0, 1) = transform.m01_;
	eigenRotation(0, 2) = transform.m02_;
	eigenRotation(1, 0) = transform.m10_;
	eigenRotation(1, 1) = transform.m11_;
	eigenRotation(1, 2) = transform.m12_;
	eigenRotation(2, 0) = transform.m20_;
	eigenRotation(2, 1) = transform.m21_;
	eigenRotation(2, 2) = transform.m22_;

	eigenTranslation(0) = transform.m03_;
	eigenTranslation(1) = transform.m13_;
	eigenTranslation(2) = transform.m23_;

	rotation = eigenRotation;
	translation = eigenTranslation;

}