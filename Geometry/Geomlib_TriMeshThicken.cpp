#include "Geomlib_TriMeshThicken.h"

#include <iostream>

#include <Eigen/Core>

#pragma warning(push, 0)
#include <igl/is_boundary_edge.h>
#include <igl/edge_flaps.h>
#include <igl/edges.h>
#pragma warning(pop)

#include <Urho3D/Core/Variant.h>

#include "ConversionUtilities.h"
#include "TriMesh.h"

#pragma warning(disable : 4244)

using Urho3D::Variant;
using Urho3D::VariantMap;
using Urho3D::VariantVector;
using Urho3D::Vector3;

bool Geomlib::TriMeshThicken(
	const Urho3D::Variant& meshIn,
	float d,
	Urho3D::Variant& meshOut
)
{
	// meshIn: verify and parse
	if (!TriMesh_Verify(meshIn)) {
		std::cerr << "ERROR: Geomlib::Thicken --- meshIn unverified\n";
		meshOut = Variant();
		return false;
	}
	VariantMap meshMap = meshIn.GetVariantMap();
	const VariantVector vertexList = meshMap["vertices"].GetVariantVector();
	const VariantVector faceList = meshMap["faces"].GetVariantVector();

	// Compute vertices for outer part of solid, using unit normals
	VariantVector vertNormals = TriMesh_ComputeVertexNormals(meshIn, true);
	assert(vertNormals.Size() == vertexList.Size());
	VariantVector outerVertexList;
	for (unsigned i = 0; i < vertexList.Size(); ++i) {
		Vector3 vert = vertexList[i].GetVector3();
		Vector3 unorm = vertNormals[i].GetVector3();

		Vector3 newVert = vert + d * unorm;
		outerVertexList.Push(Variant(newVert));
	}
	// Compute faces for outer part of solid
	// (using convention that outer vertices are listed in the 2nd half of final vertex list
	VariantVector outerFaceList;
	unsigned numVertices = vertexList.Size();
	for (unsigned i = 0; i < faceList.Size(); ++i) {
		unsigned v = faceList[i].GetUInt();
		outerFaceList.Push(numVertices + faceList[i].GetUInt());
	}

	// Compute faces to form panels connecting exposed edges
	// on inner and outer surfaces.
	VariantVector sideFaceList;

	// Determine which edges are boundary edges, using libigl
	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	bool matrixSuccess = IglMeshToMatrices(meshIn, V, F);
	assert(matrixSuccess);
	Eigen::MatrixXi EE; // EE is used to precompute # of edges, then is discarded
	igl::edges(F, EE);
	Eigen::MatrixXi E, EI;
	Eigen::MatrixXi EF(EE.rows(), 2);
	EF = Eigen::MatrixXi::Constant(EE.rows(), 2, -1); // we use the -1 default values to help identify boundary edges
	Eigen::VectorXi EMAP;
	igl::edge_flaps(F, E, EMAP, EF, EI);
	Eigen::VectorXi B;
	igl::is_boundary_edge(E, F, B);
	assert(E.rows() == B.rows());

	// Loop over boundary edges, constructing "thickened" outer edges
	unsigned numFaces = F.rows();
	for (unsigned e = 0; e < B.rows(); ++e) {
		if (B(e) == 1) {
			// Boundary found! Edge e is a boundary edge.
			// E
			unsigned f0 = EF(e, 0);
			unsigned f1 = EF(e, 1);

			assert(
				(f0 == -1 && (f1 >= 0 && f1 < numFaces)) ||
				((f0 >= 0 && f0 < numFaces) && f1 == -1)
			);

			unsigned v0 = -1;
			unsigned v1 = -1;

			if (f0 == -1) {
				// f1 is the face with boundary edge e opposite vertex EI(e, 1)
				unsigned v = EI(e, 1);

				v0 = F(f1, (v + 1) % 3);
				v1 = F(f1, (v + 2) % 3);
			}
			else if (f1 == -1) {
				// f0 is the face with boundary edge e opposite vertex EI(e, 0)
				unsigned v = EI(e, 0);

				v0 = F(f0, (v + 1) % 3);
				v1 = F(f0, (v + 2) % 3);
			}
			else {
				meshOut = Variant();
				return false;
			}

			// v0, v1: indices into vertexList of vertices (in order) of edge on the boundary
			unsigned v2 = v0 + vertexList.Size();
			unsigned v3 = v1 + vertexList.Size();

			sideFaceList.Push(Variant(v0));
			sideFaceList.Push(Variant(v1));
			sideFaceList.Push(Variant(v2));
			//
			sideFaceList.Push(Variant(v2));
			sideFaceList.Push(Variant(v1));
			sideFaceList.Push(Variant(v3));
		}
	}

	VariantVector innerFaceList;
	for (unsigned i = 0; i < faceList.Size(); i += 3) {
		innerFaceList.Push(faceList[i]);
		innerFaceList.Push(faceList[i + 2]);
		innerFaceList.Push(faceList[i + 1]);
	}

	// Collect all the inner, outer, and side panel data
	/*
	Speculative: if d < 0, reverse all orientations....
	*/

	VariantVector newVertexList = vertexList;
	newVertexList.Push(outerVertexList);
	Variant newVertices(newVertexList);

	VariantVector newFaceList = innerFaceList;
	newFaceList.Push(outerFaceList);
	newFaceList.Push(sideFaceList);
	if (d < 0.0f) {
		for (unsigned i = 0; i < newFaceList.Size(); i += 3) {
			int i1 = newFaceList[i + 1].GetInt();
			int i2 = newFaceList[i + 2].GetInt();

			newFaceList[i + 1] = Variant(i2);
			newFaceList[i + 2] = Variant(i1);
		}
	}
	Variant newFaces(newFaceList);

	meshOut = TriMesh_Make(newVertices, newFaces);
	return true;
}