#include "StockGeometries.h"

#include <math.h>

#include <algorithm>
#include <iostream>
#include <vector>

#pragma warning(push, 0)
#include <igl/adjacency_list.h>
#include <igl/edges.h>
#include <igl/edge_flaps.h>
#pragma warning(pop)

#include "TriMesh.h"
#include "Polyline.h"
#include "NMesh.h"

#pragma warning(disable : 4244)

using Urho3D::Equals;
using Urho3D::Variant;
using Urho3D::VariantVector;
using Urho3D::Vector;
using Urho3D::Vector3;

namespace {

bool Vector3Equals(const Vector3& lhs, const Vector3& rhs)
{
	return Equals(lhs.x_, rhs.x_) && Equals(lhs.y_, rhs.y_) && Equals(lhs.z_, rhs.z_);
}

void RemoveDuplicates(
	const Vector<Vector3>& vertexListIn,
	Variant& vertices,
	Variant& faces
)
{
	int numVerts = (int)vertexListIn.Size();
	std::vector<int> vertexDuplicates_(numVerts);

	for (int i = 0; i < numVerts; ++i) {
		vertexDuplicates_[i] = i; // Assume not a duplicate
		for (int j = 0; j < i; ++j) {
			if (Vector3Equals(vertexListIn[j], vertexListIn[i])) {
				vertexDuplicates_[i] = j;
				break;
			}
		}
	}

	std::vector<int> vertexUniques_ = vertexDuplicates_;
	std::sort(vertexUniques_.begin(), vertexUniques_.end());
	vertexUniques_.erase(std::unique(vertexUniques_.begin(), vertexUniques_.end()), vertexUniques_.end());

	Vector<Variant> newVertexList;
	for (int i = 0; i < (int)vertexUniques_.size(); ++i) {
		int k = vertexUniques_[i];
		newVertexList.Push(Variant(vertexListIn[k]));
	}
	vertices = Variant(newVertexList);

	Vector<Variant> faceList;
	for (int j = 0; j < numVerts; ++j) {
		int orig_j = vertexDuplicates_[j];
		int uniq_j = find(vertexUniques_.begin(), vertexUniques_.end(), orig_j) - vertexUniques_.begin();
		faceList.Push(Variant(uniq_j));
	}
	faces = Variant(faceList);
}

void subdivide_icosahedron(
	const Eigen::MatrixXf& V,
	const Eigen::MatrixXi& F,
	Eigen::MatrixXf& NV,
	Eigen::MatrixXi& NF
)
{
	unsigned numVertices = V.rows();

	float t = sqrt(0.5f * (5 + (float)sqrt(5)));

	// precompute the number of edges
	Eigen::MatrixXi EE;
	//igl::all_edges(F, EE);
	igl::edges(F, EE);

	// precompute edge-face relationship data
	Eigen::MatrixXi E, EI;
	Eigen::MatrixXi EF = Eigen::MatrixXi::Constant(EE.rows(), 2, -1);
	Eigen::VectorXi EMAP;
	igl::edge_flaps(F, E, EMAP, EF, EI);

	// precompute adjacency data
	std::vector<std::vector<int> > adjacencyData;
	igl::adjacency_list(F, adjacencyData);

	//
	NV.resize(V.rows() + E.rows(), 3);
	NV.block(0, 0, numVertices, 3) = V;

	// add points on each edge
	for (unsigned e = 0; e < E.rows(); ++e) {

		int v0 = E(e, 0);
		int v1 = E(e, 1);

		Eigen::RowVector3f mid = 0.5f * (V.row(v0) + V.row(v1));
		mid.normalize();
		mid *= t;

		NV.row(numVertices + e) = mid;
	}

	// Subdivide face data
	NF.resize(4 * F.rows(), 3);
	for (unsigned f = 0; f < F.rows(); ++f) {

		int v0 = V.rows() + EMAP(f);
		int v1 = V.rows() + EMAP(F.rows() + f);
		int v2 = V.rows() + EMAP(2 * F.rows() + f);

		NF.row(4 * f) = Eigen::RowVector3i(v0, v1, v2);
		NF.row(4 * f + 1) = Eigen::RowVector3i(F(f, 0), v2, v1);
		NF.row(4 * f + 2) = Eigen::RowVector3i(v2, F(f, 1), v0);
		NF.row(4 * f + 3) = Eigen::RowVector3i(v1, v0, F(f, 2));
	}
}

} // namespace

Urho3D::Variant MakeHexayurtMesh(float s)
{
	if (s <= 0.0f) {
		std::cerr << "ERROR: MakeHexayurtMesh --- s > 0.0f is required\n";
		return Variant();
	}

	float r3 = sqrt(3.0f);
	float t = s * r3;

	float cyc[12] = {
		-s, -t, s, -t, 2 * s, 0, s, t, -s, t, -2 * s, 0
	};

	Vector3 pk(0, 2 * s, 0);

	Vector<Vector3> vertexCoords;
	for (unsigned i = 0; i < 6; ++i) {
		Vector3 v0(cyc[2 * i], 0, cyc[2 * i + 1]);
		unsigned j = (i + 1) % 6;
		Vector3 v1(cyc[2 * j], 0, cyc[2 * j + 1]);
		Vector3 v2 = v1 + Vector3(0, s, 0);
		Vector3 v3 = v0 + Vector3(0, s, 0);
		vertexCoords.Push(v1); vertexCoords.Push(v0); vertexCoords.Push(v2);
		vertexCoords.Push(v2); vertexCoords.Push(v0); vertexCoords.Push(v3);

		Vector3 m = 0.5f * (v2 + v3);
		vertexCoords.Push(v2); vertexCoords.Push(m); vertexCoords.Push(pk);
		vertexCoords.Push(m); vertexCoords.Push(v3); vertexCoords.Push(pk);
	}

	Variant vertices, faces;
	RemoveDuplicates(vertexCoords, vertices, faces);

	return TriMesh_Make(vertices, faces);
}

Urho3D::Variant MakeCubeMesh(float s)
{
	if (s <= 0.0f) {
		std::cerr << "ERROR: MakeCubeMesh --- s > 0.0f is required\n";
		return Variant();
	}

	float t = 0.5f * s;

	float cyc[8] = { t, t, -t, t, -t, -t, t, -t };

	Vector<Vector3> vertexCoords;
	for (unsigned i = 0; i < 4; ++i) {
		Vector3 v0(cyc[2 * i], 0, cyc[2 * i + 1]);
		unsigned j = (i + 1) % 4;
		Vector3 v1(cyc[2 * j], 0, cyc[2 * j + 1]);
		Vector3 v2 = v1 + Vector3(0, 2 * t, 0);
		Vector3 v3 = v0 + Vector3(0, 2 * t, 0);
		vertexCoords.Push(v1); vertexCoords.Push(v0); vertexCoords.Push(v2);
		vertexCoords.Push(v2); vertexCoords.Push(v0); vertexCoords.Push(v3);
	}

	{
		Vector3 v0(cyc[0], 0, cyc[1]);
		Vector3 v1(cyc[2], 0, cyc[3]);
		Vector3 v2(cyc[4], 0, cyc[5]);
		Vector3 v3(cyc[6], 0, cyc[7]);
		vertexCoords.Push(v0); vertexCoords.Push(v1); vertexCoords.Push(v2);
		vertexCoords.Push(v0); vertexCoords.Push(v2); vertexCoords.Push(v3);

		Vector3 up(0, 2 * t, 0);
		v0 += up;
		v1 += up;
		v2 += up;
		v3 += up;
		vertexCoords.Push(v1); vertexCoords.Push(v0); vertexCoords.Push(v2);
		vertexCoords.Push(v2); vertexCoords.Push(v0); vertexCoords.Push(v3);
	}

	Variant vertices, faces;
	RemoveDuplicates(vertexCoords, vertices, faces);

	return TriMesh_Make(vertices, faces);
}

Urho3D::Variant MakeZigZag(float s, int n)
{
	if (n < 1) {
		std::cerr << "ERROR: MakeZigZag --- n < 1\n";
		return Variant();
	}
	if (s <= 0.0f) {
		std::cerr << "ERROR: MakeZigZag --- s <= 0.0f\n";
		return Variant();
	}

	VariantVector vertexList;
	vertexList.Push(Vector3(0.0f, 0.0f, 0.0f));

	float x = 0.0f;
	float z = 0.0f;
	for (int i = 0; i < n; ++i) {
		x += s;
		z = (i % 2 == 0) ? s : 0.0f;
		vertexList.Push(Vector3(x, 0.0f, z));
	}

	return Polyline_Make(vertexList);
}

bool MakeZigZag(float s, float t, int n, Urho3D::Variant& zigZagPolyline)
{
	if (n < 1) {
		std::cerr << "ERROR: MakeZigZag --- n < 1\n";
		zigZagPolyline = Variant();
		return false;
	}
	if (s == 0.0f && t == 0.0f) {
		std::cerr << "ERROR: MakeZigZag --- s == 0.0f && t == 0.0f\n";
		zigZagPolyline = Variant();
		return false;
	}

	VariantVector vertexList;
	vertexList.Push(Vector3(0.0f, 0.0f, 0.0f));

	float x = 0.0f;
	float z = 0.0f;
	for (int i = 0; i < n; ++i) {
		x += s;
		z = (i % 2 == 0) ? t : 0.0f;
		vertexList.Push(Vector3(x, 0.0f, z));
	}

	zigZagPolyline = Polyline_Make(vertexList);
	return true;
}

Urho3D::Variant MakeIcosahedron()
{
	float t = (1 + (float)sqrt(5)) / 2;

	Eigen::MatrixXf V(12, 3);
	V <<
		t, 1, 0, // 0
		1, 0, t, // 1, A
		0, t, 1, // 2, B
		t, -1, 0, // 3, E
		-1, 0, t, // 4, DD <-> A
		0, t, -1, // 5, C
		-t, 1, 0, // 6, EE <-> B
		1, 0, -t, // 7, D
		0, -t, 1, // 8, CC <-> E
		-t, -1, 0, // 9
		-1, 0, -t, // 10, AA <-> C
		0, -t, -1; // 11, BB <-> D

	Eigen::MatrixXi F(20, 3);
	F <<
		1, 2, 4,
		2, 5, 6,
		5, 7, 10,
		7, 3, 11,
		3, 1, 8, //
		10, 7, 11,
		11, 3, 8,
		8, 1, 4,
		4, 2, 6,
		6, 5, 10, //
		0, 2, 1,
		0, 5, 2,
		0, 7, 5,
		0, 3, 7,
		0, 1, 3, //
		9, 10, 11,
		9, 11, 8,
		9, 8, 4,
		9, 4, 6,
		9, 6, 10;

	return TriMesh_Make(V, F);
}

Urho3D::Variant MakeSphere()
{
	float t = (1 + (float)sqrt(5)) / 2;
	Eigen::MatrixXf V(12, 3);
	V <<
		t, 1, 0, // 0
		1, 0, t, // 1, A
		0, t, 1, // 2, B
		t, -1, 0, // 3, E
		-1, 0, t, // 4, DD <-> A
		0, t, -1, // 5, C
		-t, 1, 0, // 6, EE <-> B
		1, 0, -t, // 7, D
		0, -t, 1, // 8, CC <-> E
		-t, -1, 0, // 9
		-1, 0, -t, // 10, AA <-> C
		0, -t, -1; // 11, BB <-> D

	Eigen::MatrixXi F(20, 3);
	F <<
		1, 2, 4,
		2, 5, 6,
		5, 7, 10,
		7, 3, 11,
		3, 1, 8, //
		10, 7, 11,
		11, 3, 8,
		8, 1, 4,
		4, 2, 6,
		6, 5, 10, //
		0, 2, 1,
		0, 5, 2,
		0, 7, 5,
		0, 3, 7,
		0, 1, 3, //
		9, 10, 11,
		9, 11, 8,
		9, 8, 4,
		9, 4, 6,
		9, 6, 10;

	Eigen::MatrixXf V2, NV;
	Eigen::MatrixXi F2, NF;
	subdivide_icosahedron(V, F, V2, F2);
	subdivide_icosahedron(V2, F2, NV, NF);

	return TriMesh_Make(NV, NF);
}

Urho3D::Variant MakeRegularPolygon(int n)
{
	if (n <= 2) {
		return Variant();
	}

	float theta = 2 * 3.14159f / n;

	VariantVector vertex_list;

	Vector3 v0(1.0f, 0.0f, 0.0f);
	vertex_list.Push(Variant(v0));

	for (int i = 1; i < n; ++i) {
		float x = std::cos(i * theta);
		float z = std::sin(i * theta);
		Vector3 v(x, 0.0f, z);
		vertex_list.Push(Variant(v));
	}

	vertex_list.Push(Variant(v0));

	return Polyline_Make(vertex_list);
}

Urho3D::Variant MakeSuperTorus(float outer_radius, float inner_radius, float first_power, float second_power, int res)
{
	// regard the torus as a rectangle, with opposite sides identified. 
	// res gives the number of divisions in each direction. So the rectangle will be divded into res*res quads
	// to create the vertex list, we create the vertices row by row.
	// example: res = 4
	// v_03, v_13, v_23, v_33
	// v_02, v_12, v_22, v_32
	// v_01, v_11, v_21, v_31
	// v_00, v_10, v_20, v_30


	// so the translation to vertex IDs is 
	// 12 13 14 15
	// 8 9 10 11
	// 4 5 6 7
	// 0 1 2 3 


	// the faces may then be created from this list. 
	// see http://paulbourke.net/geometry/torus/source2.c

	float DTOR = 0.01745329252;
	float n1 = first_power;
	float n2 = second_power;
	float r0 = outer_radius;
	float r1 = inner_radius;

	// make the vertex list
	VariantVector vertex_list;

    /*
	int dx = (int)360 / res;
	for (int u = 0; u < 360; u += dx) {
		for (int v = 0; v < 360; v += dx) {
			float theta = u*DTOR;
			float phi = v*DTOR;

			Vector3 curVert;
			curVert.x_ = pow(cos(theta), n1) * (r0 + r1 * pow(cos(phi), n2));
			curVert.y_ = pow(sin(theta), n1) * (r0 + r1 * pow(cos(phi), n2));
			curVert.z_ = r1 * pow(sin(phi), n2);
			vertex_list.Push(Variant(curVert));
		}
	}
     */
    
    float dx = 360.0f / res;
    for (int u = 0; u < res; ++u) {
        for (int v = 0; v < res; ++v) {
            float theta = u*dx*DTOR;
            float phi = v*dx*DTOR;
            
            Vector3 curVert;
            curVert.x_ = pow(cos(theta), n1) * (r0 + r1 * pow(cos(phi), n2));
            curVert.y_ = pow(sin(theta), n1) * (r0 + r1 * pow(cos(phi), n2));
            curVert.z_ = r1 * pow(sin(phi), n2);
            vertex_list.Push(Variant(curVert));
        }
    }

	VariantVector face_list;
    VariantVector tri_face_list;
	// make the face list
	for (int i = 0; i < res; ++i) {
		for (int j = 0; j < res; ++j) {
			// all faces are quads:
			face_list.Push(4);

			int lower_left = res*i + j;
			int lower_right = res*i + ((j + 1) % res);
			int upper_right = res*(i+1)%(res*res) + ((j + 1) % res);
			int upper_left = res*(i + 1) % (res*res) + j;

            
            face_list.Push(upper_left);
            face_list.Push(upper_right);
            face_list.Push(lower_right);
			face_list.Push(lower_left);
            
            // making tri_face 1
            tri_face_list.Push(lower_left);
            tri_face_list.Push(lower_right);
            tri_face_list.Push(upper_right);
            
            // making tri_face 2
            tri_face_list.Push(lower_left);
            tri_face_list.Push(upper_right);
            tri_face_list.Push(upper_left);

		}
	}

	Urho3D::Variant superTorus = NMesh_Make(vertex_list, face_list);
    
    Urho3D::Variant torus = TriMesh_Make(vertex_list, tri_face_list);

	return superTorus;
    //return torus;
}
