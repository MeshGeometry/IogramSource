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
#include <Urho3D/AngelScript/APITemplates.h>



// TriMeshWithData is a variant map:
/*
 ["type"] = TriMeshWithData
 ["mesh"] = VariantMap
 ["vertex-vertex"] = VariantVector (with Vector<Vector<int>> structure)
 ["vertex-face"] = VariantVector (with Vector<Vector<int>> structure)
 ["face-face"] = VariantVector
 
 */

// Leaving out edges and labels for now
// ASSUMES MANIFOLD



// this computes all the various adjacency data and stores it as an enhanced triMesh
Urho3D::Variant TriMesh_ComputeAdjacencyData(const Urho3D::Variant& triMesh); // REGISTERED

// this checks for existance, but makes no guarantee that the data is uptodate.
bool TriMesh_HasAdjacencyData(const Urho3D::Variant& triMeshWithData); // REGISTERED


/// VERTEX QUERIES
Urho3D::VariantVector TriMesh_VertexToVertices(Urho3D::Variant& triMeshWithData, int vertID); // REGISTERED as TriMesh_VertexToVerticesArrayFromId
Urho3D::Vector<Urho3D::Variant> TriMesh_VertexToVertices(Urho3D::Variant& triMeshWithData); // REGISTERED as TriMesh_VertexToVerticesArray
Urho3D::VariantVector TriMesh_VertexToFaces(Urho3D::Variant& triMeshWithData, int vertID); // REGISTERED as TriMesh_VertexToFacesArray
//Urho3D::VariantVector TriMesh_VertexToLabels(const Urho3D::Variant& triMesh, int vertID);

/// FACE QUERIES
Urho3D::VariantVector TriMesh_FaceToVertices(const Urho3D::Variant& triMeshWithData, int faceID); // REGISTERED as TriMesh_FaceToVerticesArray
Urho3D::VariantVector TriMesh_FaceToFaces(const Urho3D::Variant& triMeshWithData, int faceID); // REGISTERED as TriMesh_FaceToFacesArray
//Urho3D::VariantVector TriMesh_FaceToLabels(const Urho3D::Variant& triMesh, int faceID);

// TODO
/// LABEL QUERIES
//Urho3D::VariantVector TriMesh_LabelToVertices(const Urho3D::Variant& triMesh, int faceID);
//Urho3D::VariantVector TriMesh_LabelToFaces(const Urho3D::Variant& triMesh, int faceID);
//Urho3D::VariantVector TriMesh_LabelToLabels(const Urho3D::Variant& triMesh, int faceID);

// for scripts
Urho3D::CScriptArray* TriMesh_VertexToVerticesArrayFromId(Urho3D::Variant& triMeshWithData, int vertID);
Urho3D::CScriptArray* TriMesh_VertexToVerticesArray(Urho3D::Variant& triMeshWithData);
Urho3D::CScriptArray* TriMesh_VertexToFacesArray(Urho3D::Variant& triMeshWithData, int vertID);
Urho3D::CScriptArray* TriMesh_FaceToVerticesArray(const Urho3D::Variant& triMeshWithData, int faceID);
Urho3D::CScriptArray* TriMesh_FaceToFacesArray(const Urho3D::Variant& triMeshWithData, int faceID);

bool RegisterMeshTopologyQueryFunctions(Urho3D::Context* context);
