#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>

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
Urho3D::Variant TriMesh_ComputeAdjacencyData(const Urho3D::Variant& triMesh);

// this checks for existance, but makes no guarantee that the data is uptodate.
bool TriMesh_HasAdjacencyData(const Urho3D::Variant& triMeshWithData);


/// VERTEX QUERIES
Urho3D::VariantVector TriMesh_VertexToVertices(Urho3D::Variant& triMeshWithData, int vertID);
Urho3D::Vector<Urho3D::Variant> TriMesh_VertexToVertices(Urho3D::Variant& triMeshWithData);
Urho3D::VariantVector TriMesh_VertexToFaces(Urho3D::Variant& triMeshWithData, int vertID);
//Urho3D::VariantVector TriMesh_VertexToLabels(const Urho3D::Variant& triMesh, int vertID);

/// FACE QUERIES
Urho3D::VariantVector TriMesh_FaceToVertices(const Urho3D::Variant& triMeshWithData, int faceID);
Urho3D::VariantVector TriMesh_FaceToFaces(const Urho3D::Variant& triMeshWithData, int faceID);
//Urho3D::VariantVector TriMesh_FaceToLabels(const Urho3D::Variant& triMesh, int faceID);

// TODO
/// LABEL QUERIES
//Urho3D::VariantVector TriMesh_LabelToVertices(const Urho3D::Variant& triMesh, int faceID);
//Urho3D::VariantVector TriMesh_LabelToFaces(const Urho3D::Variant& triMesh, int faceID);
//Urho3D::VariantVector TriMesh_LabelToLabels(const Urho3D::Variant& triMesh, int faceID);
