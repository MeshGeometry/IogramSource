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


#pragma once

#include <Eigen/Core>

#include <Urho3D/Core/Variant.h>

// convert matrix from float to double or double to float
// since some libigl functions require type double
Eigen::MatrixXd IglFloatToDouble(
	const Eigen::MatrixXf& V
);
Eigen::MatrixXf IglDoubleToFloat(
	const Eigen::MatrixXd& V
);


Urho3D::VariantVector IglComputeFaceNormals(
	const Eigen::MatrixXf& V,
	const Eigen::MatrixXi& F
);
Urho3D::VariantVector IglComputeVertexNormals(
	const Eigen::MatrixXf& V,
	const Eigen::MatrixXi& F
);

//////////////////////////////////////////////
// preferred libigl -> Variant mesh conversion
Urho3D::Variant IglMatricesToMesh(
	const Eigen::MatrixXf& V,
	const Eigen::MatrixXi& F
);
//////////////////////////////////////////////
// preferred Variant mesh -> libigl conversion
bool IglMeshToMatrices(
	const Urho3D::Variant& mesh,
	Eigen::MatrixXf& V,
	Eigen::MatrixXi& F
);

// If data stored in mesh passes basic tests then this function essentially does
//   vertexList = mesh.GetVariantMap()["vertices"].GetVariantVector();
//   faceList = mesh.GetVariantMap()["faces"].GetVariantVector();
// and returns true;
// Otherwise Clears() vertexList and faceList and returns false.
// Basic tests are
//   (1) non-zero vertex count
//   (2) non-zero face count; consistent face count, i.e., list length divisible by 3
//   (3) all high level VariantType values are correct, that is,
//       mesh stores a VariantMap with the correct keys, etc.
//   (4) vertexList[i].GetVariantType() == VAR_VECTOR3 for all i
//   (5) faceList[i].GetVariantType() == VAR_INT for all i
//   (6) consistent face data -- no repeated indices
bool ExtractMeshData(
	const Urho3D::Variant& mesh,
	Urho3D::VariantVector& vertexList,
	Urho3D::VariantVector& faceList
);

// Conversion from Urho Matrix3X4 to Eigen::MatrixXf (3X3) rotation/scaling matrix and Eigen::RowVector3f translation vec
void Matrix3x4ToEigen(const Urho3D::Matrix3x4& transform, Eigen::MatrixXf& rotation, Eigen::RowVector3f& translation);
