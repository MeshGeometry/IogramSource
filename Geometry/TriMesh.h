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

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Graphics/VertexBuffer.h>
#include <Urho3D/Graphics/IndexBuffer.h>
#include <Urho3D/Graphics/Model.h>

#include <Eigen/Core>

Urho3D::Variant TriMesh_Make(const Eigen::MatrixXf& V, const Eigen::MatrixXi& F);
Urho3D::Variant TriMesh_Make(const Urho3D::Variant& vertices, const Urho3D::Variant& faces);
Urho3D::Variant TriMesh_Make(const Urho3D::VariantVector& vertexList, const Urho3D::VariantVector& faceList);
Urho3D::Variant TriMesh_Make(const Urho3D::VariantVector& vertexList, const Urho3D::VariantVector& faceList, 
	const Urho3D::VariantVector& labelList);

bool TriMesh_Verify(const Urho3D::Variant& triMesh);

Urho3D::VariantVector TriMesh_GetVertexList(const Urho3D::Variant& triMesh);
Urho3D::VariantVector TriMesh_GetFaceList(const Urho3D::Variant& triMesh);
Urho3D::VariantVector TriMesh_GetNormalList(const Urho3D::Variant& triMesh);
Urho3D::VariantVector TriMesh_GetLabelList(const Urho3D::Variant& triMesh);

Urho3D::VariantVector TriMesh_ComputeFaceNormals(const Urho3D::Variant& triMesh, bool normalize = false);
Urho3D::VariantVector TriMesh_ComputeVertexNormals(const Urho3D::Variant& triMesh, bool normalize = false);
Urho3D::Vector<Urho3D::Pair<int, int>> TriMesh_ComputeEdges(const Urho3D::Variant& triMesh);
Urho3D::Vector<float> TriMesh_GetVerticesAsFloats(const Urho3D::Variant& triMesh);
Urho3D::Vector<double> TriMesh_GetVerticesAsDoubles(const Urho3D::Variant& triMesh);
Urho3D::Vector<int> TriMesh_GetFacesAsInts(const Urho3D::Variant& triMesh);

Urho3D::Vector<Urho3D::Pair<int, int>> TriMesh_ComputeBoundaryEdges(const Urho3D::Variant& triMesh);

Urho3D::Vector<Urho3D::Vector3> TriMesh_ComputePointCloud(const Urho3D::Variant& triMesh);

Urho3D::Variant TriMesh_ApplyTransform(
	const Urho3D::Variant& tri_mesh,
	const Urho3D::Matrix3x4& T
);

Urho3D::Variant TriMesh_CullUnusedVertices(const Urho3D::Variant& tri_mesh);

Urho3D::Variant TriMesh_DoubleAndFlipFaces(const Urho3D::Variant& tri_mesh);

Urho3D::Variant TriMesh_BoundingBox(const Urho3D::Variant& tri_mesh);

Urho3D::Variant TriMesh_UnifyNormals(const Urho3D::Variant& tri_mesh);

Urho3D::Variant TriMesh_FlipNormals(const Urho3D::Variant& tri_mesh);

void TriMeshToMatrices(const Urho3D::Variant& triMesh, Eigen::MatrixXf& V, Eigen::MatrixXi& F);
void TriMeshToDoubleMatrices(const Urho3D::Variant& triMesh, Eigen::MatrixXd& V, Eigen::MatrixXi& F);
//Display functions
Urho3D::Model* TriMesh_GetRenderMesh(const Urho3D::Variant& triMesh, Urho3D::Context* context, Urho3D::VariantVector vColors, bool split=false);
