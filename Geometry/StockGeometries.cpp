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

#include "StockGeometries.h"

#include <math.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include <Urho3D/IO/Log.h>

#pragma warning(push, 0)
#include <igl/adjacency_list.h>
#include <igl/edges.h>
#include <igl/edge_flaps.h>
#pragma warning(pop)

#include "TriMesh.h"
#include "Polyline.h"
#include "NMesh.h"
#include "Geomlib_TriangulatePolygon.h"
#include "Geomlib_TriMeshThicken.h"

#include <Urho3D/AngelScript/Script.h>
#include <AngelScript/angelscript.h>


#pragma warning(disable : 4244)

#define CHECK_GEO_REG(result) if (result <= 0) { \
		printf("geo_reg: FAIL\n"); \
		failed = true; \
	}

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

Urho3D::Variant MakeCubeMeshFromExtremeCorners(Vector3 m, Vector3 M)
{
	float cyc[8] = { M.x_, M.z_, m.x_, M.z_, m.x_, m.z_, M.x_, m.z_ };

	Vector<Vector3> vertexCoords;
	for (unsigned i = 0; i < 4; ++i) {
		Vector3 v0(cyc[2 * i], m.y_, cyc[2 * i + 1]);
		unsigned j = (i + 1) % 4;
		Vector3 v1(cyc[2 * j], m.y_, cyc[2 * j + 1]);
		Vector3 v2 = v1 + Vector3(0, M.y_ - m.y_, 0);
		Vector3 v3 = v0 + Vector3(0, M.y_ - m.y_, 0);
		vertexCoords.Push(v1); vertexCoords.Push(v0); vertexCoords.Push(v2);
		vertexCoords.Push(v2); vertexCoords.Push(v0); vertexCoords.Push(v3);
	}

	{
		Vector3 v0(cyc[0], m.y_, cyc[1]);
		Vector3 v1(cyc[2], m.y_, cyc[3]);
		Vector3 v2(cyc[4], m.y_, cyc[5]);
		Vector3 v3(cyc[6], m.y_, cyc[7]);
		vertexCoords.Push(v0); vertexCoords.Push(v1); vertexCoords.Push(v2);
		vertexCoords.Push(v0); vertexCoords.Push(v2); vertexCoords.Push(v3);

		Vector3 up(0, M.y_ - m.y_, 0);
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

Urho3D::Variant MakeHelix(float radius, float height, float turns, int res)
{
   
    VariantVector vertexList;    
    float height_incr= height/res;

	if (turns == 0 || res == 0) {
		URHO3D_LOGERROR("Number of turns or resolution may not be zero ");
		return Variant();
	}

	float incr = 2*M_PI / ((float)res / (float)turns);
    
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    for (int i = 0; i < res+1; ++i) {
        x = radius*cos(i*incr);
        y = i*height_incr;
        z = radius*sin(i*incr);
        vertexList.Push(Vector3(x, y, z));
    }
    
    return Polyline_Make(vertexList);
}

Urho3D::Variant MakeSpiral(float lower_radius, float upper_radius, float height, float turns, int res)
{

	VariantVector vertexList;

	float height_incr = height / res;
	if (lower_radius == upper_radius) {
		URHO3D_LOGERROR("Lower radius must not be the same as the upper radius");
		return Variant();
	}

	if (turns == 0 || res == 0) {
		URHO3D_LOGERROR("Number of turns or resolution may not be zero ");
		return Variant();
	}

	float incr = 2 * M_PI / ((float)res / (float)turns);
	float radius_incr = (upper_radius - lower_radius) / (float)res;

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	for (int i = 0; i < res + 1; ++i) {
		float radius = lower_radius + i*radius_incr;
		x = radius*cos(i*incr);
		y = i*height_incr;
		z = radius*sin(i*incr);
		vertexList.Push(Vector3(x, y, z));
	}

	return Polyline_Make(vertexList);
}

Urho3D::Variant MakeIcosahedron(float r)
{
	float t = r*(1 + (float)sqrt(5)) / 2;

	Eigen::MatrixXf V(12, 3);
	V <<
		t, r, 0, // 0
		r, 0, t, // 1, A
		0, t, r, // 2, B
		t, -r, 0, // 3, E
		-r, 0, t, // 4, DD <-> A
		0, t, -r, // 5, C
		-t, r, 0, // 6, EE <-> B
		r, 0, -t, // 7, D
		0, -t, r, // 8, CC <-> E
		-t, -r, 0, // 9
		-r, 0, -t, // 10, AA <-> C
		0, -t, -r; // 11, BB <-> D

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
	float offset = 0.5f * theta;

	VariantVector vertex_list;

	//Vector3 v0(1.0f, 0.0f, 0.0f);
	//vertex_list.Push(Variant(v0));

	for (int i = 0; i < n; ++i) {
		float x = std::cos(i * theta + offset);
		float z = std::sin(i * theta + offset);
		Vector3 v(x, 0.0f, z);
		vertex_list.Push(Variant(v));
	}

	vertex_list.Push(vertex_list.Front());

	return Polyline_Make(vertex_list);
}

Urho3D::Variant MakeSuperTorus(Urho3D::Variant& triMesh, float outer_radius, float inner_radius, float first_power, float second_power, int res)
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
    
    float dx = 360.0f / res;
    for (int u = 0; u < res; ++u) {
        for (int v = 0; v < res; ++v) {
            float theta = u*dx*DTOR;
            float phi = v*dx*DTOR;
            
            float C_theta = copysignf(1.0f, cos(theta))*(pow(std::abs(cos(theta)), n1));
            float C_phi = copysignf(1.0f, cos(phi))*(pow(std::abs(cos(phi)), n2));
            float S_theta = copysignf(1.0f, sin(theta))*(pow(std::abs(sin(theta)), n1));
            float S_phi = copysignf(1.0f, sin(phi))*(pow(std::abs(sin(phi)), n2));
            
            Vector3 curVert;
            curVert.x_ = (r0 + C_theta) * C_phi;
            curVert.y_ = (r1 + C_theta) * S_phi;
            curVert.z_ = S_theta;
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

            face_list.Push(lower_left);
            face_list.Push(lower_right);
            face_list.Push(upper_right);
            face_list.Push(upper_left);
            
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
    
    triMesh = TriMesh_Make(vertex_list, tri_face_list);

	return superTorus;
    //return torus;
}

Urho3D::Variant MakeXZPlane(float y_coord)
{

	VariantVector vertexList;
	VariantVector faceList;

	Vector3 v_0(-100, y_coord, -100);
	Vector3 v_1(100, y_coord, -100);
	Vector3 v_2(100, y_coord, 100);
	Vector3 v_3(-100, y_coord, 100);

	vertexList.Push(v_0);
	vertexList.Push(v_1);
	vertexList.Push(v_2);
	vertexList.Push(v_3);

	faceList.Push(2);
	faceList.Push(1);
	faceList.Push(0);

	faceList.Push(3);
	faceList.Push(2);
	faceList.Push(0);

	Variant triMesh = TriMesh_Make(vertexList, faceList);

	return triMesh;
}

Urho3D::Variant MakeYZPlane(float x_coord)
{
    
    VariantVector vertexList;
    VariantVector faceList;
    
    Vector3 v_0(x_coord, -100, -100);
    Vector3 v_1(x_coord, 100,  -100);
    Vector3 v_2(x_coord, 100, 100);
    Vector3 v_3(x_coord, -100, 100);
    
    vertexList.Push(v_0);
    vertexList.Push(v_1);
    vertexList.Push(v_2);
    vertexList.Push(v_3);
    
    faceList.Push(2);
    faceList.Push(1);
    faceList.Push(0);
    
    faceList.Push(3);
    faceList.Push(2);
    faceList.Push(0);
    
    Variant triMesh = TriMesh_Make(vertexList, faceList);
    
    return triMesh;
}

Urho3D::Variant MakeXYPlane(float z_coord)
{
    
    VariantVector vertexList;
    VariantVector faceList;
    
    Vector3 v_0(-100,  -100, z_coord);
    Vector3 v_1(100,  -100, z_coord);
    Vector3 v_2(100,  100, z_coord);
    Vector3 v_3(-100,  100, z_coord);
    
    vertexList.Push(v_0);
    vertexList.Push(v_1);
    vertexList.Push(v_2);
    vertexList.Push(v_3);
    
    faceList.Push(2);
    faceList.Push(1);
    faceList.Push(0);
    
    faceList.Push(3);
    faceList.Push(2);
    faceList.Push(0);
    
    Variant triMesh = TriMesh_Make(vertexList, faceList);
    
    return triMesh;
}

Urho3D::Variant MakeCone(int sides, float radius, float height)
{
    if (sides <= 2) {
        return Variant();
    }
    
    float theta = 2 * 3.14159f / sides;
    
    VariantVector vertex_list;
    VariantVector face_list;
    
    Vector3 v0(0.0f, height, 0.0f);
    vertex_list.Push(Variant(v0));
    
    // verts for the base
    for (int i = 0; i < sides; ++i) {
        float x = radius*std::cos(i * theta);
        float z = radius*std::sin(i * theta);
        Vector3 v(x, 0.0f, z);
        vertex_list.Push(Variant(v));
    }
    
    // make the mesh for the base
    for (int i = 1; i < sides-1; ++i){
        face_list.Push(Variant(1));
        face_list.Push(Variant(i));
        face_list.Push(Variant(i+1));
    }
    
    // make the sides
    for (int i = 0; i < sides; ++i){
        face_list.Push(Variant(0));
        face_list.Push(Variant(i));
        face_list.Push(Variant((i+1)%sides));
    }
    Variant tri_mesh = TriMesh_Make(vertex_list, face_list);
    
    return tri_mesh;
    
}

Urho3D::Variant MakeTruncatedCone(int sides, float radius, float height)
{
    Urho3D::Variant ret;
    return ret;
}

// bot_r is the radius of the base
// top_r is the radius of the top
Urho3D::Variant MakeCylinder(int sides, float base_r, float top_r, float height)
{
    if (sides <= 2) {
        return Variant();
    }
    
    float theta = 2 * 3.14159f / sides;
    
    VariantVector vertex_list;
    VariantVector face_list;

    
    // make the base verts
    for (int i = 0; i < sides; ++i) {
        float x = base_r*std::cos(i * theta);
        float z = base_r*std::sin(i * theta);
        Vector3 v(x, 0.0f, z);
        vertex_list.Push(Variant(v));
    }
    
    // make the top verts
    for (int i = 0; i < sides; ++i) {
        float x = top_r*std::cos(i * theta);
        float z = top_r*std::sin(i * theta);
        Vector3 v(x, height, z);
        vertex_list.Push(Variant(v));
    }
    
    // make the mesh for the base
    for (int i = 1; i < sides-1; ++i){
        face_list.Push(Variant(0));
        face_list.Push(Variant(i));
        face_list.Push(Variant(i+1));
    }
    

    // make the mesh for the top
    for (int i = 1; i < sides-1; ++i){
        face_list.Push(Variant(sides + i+1));
        face_list.Push(Variant(sides + i));
        face_list.Push(Variant(sides));
    }

    
    // make the sides
    for (int i = 0; i < sides; ++i){
        //first tri
//        int a = i;
//        int b = sides + i;
//        int c = sides + (i+1)%sides;
        face_list.Push(Variant(i));
        face_list.Push(Variant(sides + i));
        face_list.Push(Variant(sides + (i+1)%sides));
        //second tri
//        int d = i;
//        int e = sides + (i+1)%sides;
//        int f = (i+1)%sides;
        face_list.Push(Variant(i));
        face_list.Push(Variant(sides + (i+1)%sides));
        face_list.Push(Variant((i+1)%sides));
    }
    
    Variant tri_mesh = TriMesh_Make(vertex_list, face_list);
    
    return tri_mesh;
}

bool MakeZigZagWithStretchParams(float s, float t, int n, Urho3D::Variant& zigZagPolyline)
{
	return MakeZigZag(s, t, n, zigZagPolyline);
}

bool RegisterStockGeometryFunctions(Urho3D::Context* context)
{
	Urho3D::Script* script_system = context->GetSubsystem<Urho3D::Script>();
	asIScriptEngine* engine = script_system->GetScriptEngine();

	bool failed = false;

	int res;

	res = engine->RegisterGlobalFunction(
		"Variant MakeHexayurtMesh(float)",
		asFUNCTION(MakeHexayurtMesh),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant MakeCubeMesh(float)",
		asFUNCTION(MakeCubeMesh),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant MakeCubeMeshFromExtremeCorners(Vector3, Vector3)",
		asFUNCTION(MakeCubeMeshFromExtremeCorners),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Variant MakeZigZagWithStretchParams(float, float, int, Variant&)",
		asFUNCTION(MakeZigZagWithStretchParams),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Variant MakeHelix(float, float, float, int)",
		asFUNCTION(MakeHelix),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant MakeSpiral(float, float, float, float, int)",
		asFUNCTION(MakeSpiral),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant MakeSphere()",
		asFUNCTION(MakeSphere),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant MakeIcosahedron(float)",
		asFUNCTION(MakeIcosahedron),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant MakeRegularPolygon(int)",
		asFUNCTION(MakeRegularPolygon),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant MakeSuperTorus(Variant&, float, float, float, float, int)",
		asFUNCTION(MakeSuperTorus),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Variant MakeXZPlane(float)",
		asFUNCTION(MakeXZPlane),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant MakeYZPlane(float)",
		asFUNCTION(MakeYZPlane),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant MakeXYPlane(float)",
		asFUNCTION(MakeXYPlane),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Variant MakePlane(float)",
		asFUNCTION(MakeXZPlane),
		asCALL_CDECL
	);

	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant MakeCone(int, float, float)",
		asFUNCTION(MakeCone),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant MakeTruncatedCone(int, float, float)",
		asFUNCTION(MakeTruncatedCone),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant MakeCylinder(int, float, float, float)",
		asFUNCTION(MakeCylinder),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	if (failed) {
		URHO3D_LOGINFO("RegisterStockGeometryFunctions --- Failed to compile scripts");
	}
	else {
		URHO3D_LOGINFO("RegisterStockGeometryFunctions --- OK!");
	}

	return !failed;
}
