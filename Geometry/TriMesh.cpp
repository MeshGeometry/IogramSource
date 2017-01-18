#include "TriMesh.h"

#include <iostream>
#include <vector>

#pragma warning(push, 0)
#include <igl/bounding_box.h>
#include <igl/vertex_triangle_adjacency.h>
#include <igl/remove_unreferenced.h>
#include <igl/bfs_orient.h>
#include <igl/edges.h>
#pragma warning(pop)

#include "ConversionUtilities.h"
#include "Geomlib_TransformVertexList.h"
#include "Geomlib_RemoveDuplicates.h"

#include <Urho3D/Math/MathDefs.h>

#pragma warning(disable : 4244)

using Urho3D::String;
using Urho3D::Variant;
using Urho3D::VariantMap;
using Urho3D::VariantType;
using Urho3D::VariantVector;
using Urho3D::Vector3;
using Urho3D::Vector;
using Urho3D::VertexBuffer;
using Urho3D::IndexBuffer;
using Urho3D::Model;
using Urho3D::Color;
using Urho3D::Pair;
//using Urho3D::FloorToInt;
using Urho3D::SharedPtr;
using Urho3D::VertexBuffer;
using Urho3D::IndexBuffer;
using Urho3D::Geometry;
using Urho3D::BoundingBox;
using Urho3D::PODVector;

namespace {

	VariantVector ExtractVertices(const Eigen::MatrixXf& V)
	{
		VariantVector earlyRet;
		if (V.rows() == 0) {
			std::cerr << "ERROR: ExtractVertices(const MatrixXf&) --- V.rows() == 0\n";
			return earlyRet;
		}
		if (V.cols() != 3) {
			std::cout << "ERROR: ExtractVertices(const MatrixXf&) --- V.cols() != 3\n";
			return earlyRet;
		}

		VariantVector vertexList;
		for (unsigned i = 0; i < V.rows(); ++i) {
			Vector3 vert(V(i, 0), V(i, 1), V(i, 2));
			vertexList.Push(Variant(vert));
		}
		return vertexList;
	}

	VariantVector ExtractFaces(const Eigen::MatrixXi& F, int numVertices)
	{
		VariantVector earlyRet;
		if (F.rows() == 0) {
			std::cout << "ERROR: ExtractFaces --- F.rows() == 0\n";
			return earlyRet;
		}
		if (F.cols() != 3) {
			std::cout << "ERROR: ExtractFaces --- F.rows() != 3\n";
			return earlyRet;
		}

		VariantVector faceList;
		for (int i = 0; i < F.rows(); ++i) {

			int i0 = F(i, 0);
			int i1 = F(i, 1);
			int i2 = F(i, 2);

			if (i0 == i1 || i1 == i2 || i2 == i0) {
				std::cerr << "ERROR: ExtractFaces --- repeated vertex indices in face\n";
				return earlyRet;
			}

			if (
				(i0 < 0 || i0 > numVertices - 1) ||
				(i1 < 0 || i1 > numVertices - 1) ||
				(i2 < 0 || i2 > numVertices - 1)
				)
			{
				std::cerr << "ERROR: ExtractFaces --- vertex index out of range\n";
				return earlyRet;
			}

			faceList.Push(Variant(i0));
			faceList.Push(Variant(i1));
			faceList.Push(Variant(i2));
		}
		return faceList;
	}


	VariantVector ComputeFaceNormals(const Eigen::MatrixXf& V, const Eigen::MatrixXi& F)
	{
		VariantVector normals;
		for (unsigned i = 0; i < F.rows(); ++i) {
			Eigen::RowVector3f u = V.row(F(i, 1)) - V.row(F(i, 0));
			Eigen::RowVector3f v = V.row(F(i, 2)) - V.row(F(i, 0));

			Eigen::RowVector3f n = u.cross(v);
			normals.Push(Vector3(n(0), n(1), n(2)));
		}

		return normals;
	}

	VariantVector ComputeFaceNormals(const VariantVector& vertexList, const VariantVector& faceList, bool normalize)
	{
		VariantVector normals;
		for (unsigned i = 0; i < faceList.Size(); i += 3) {
			unsigned i0 = faceList[i].GetInt();
			unsigned i1 = faceList[i + 1].GetInt();
			unsigned i2 = faceList[i + 2].GetInt();

			Vector3 v0 = vertexList[i0].GetVector3();
			Vector3 v1 = vertexList[i1].GetVector3();
			Vector3 v2 = vertexList[i2].GetVector3();

			Vector3 n = (v1 - v0).CrossProduct(v2 - v0);

			if (normalize) {
				n.Normalize();
			}

			normals.Push(n);
		}
		return normals;
	}

} // namespace

Urho3D::Variant TriMesh_Make(const Eigen::MatrixXf& V, const Eigen::MatrixXi& F)
{
	Variant earlyRet;

	VariantVector vertexList = ExtractVertices(V);
	if (vertexList.Size() == 0) {
		std::cerr << "ERROR: TriMesh_Make --- vertexList.Size() == 0\n";
		return earlyRet;
	}

	int numVertices = (int)V.rows();
	VariantVector faceList = ExtractFaces(F, numVertices);
	if (faceList.Size() == 0) {
		std::cerr << "ERROR: TriMesh_Make --- faceList.Size() == 0\n";
		return earlyRet;
	}

	VariantMap var_map;
	var_map["type"] = Variant(String("TriMesh"));
	var_map["vertices"] = Variant(vertexList);
	var_map["faces"] = Variant(faceList);
	var_map["normals"] = IglComputeVertexNormals(V, F);

	return Variant(var_map);
}

Urho3D::Variant TriMesh_Make(const Urho3D::VariantVector& vertexList, const Urho3D::VariantVector& faceList)
{
	Variant earlyRet;
	if (vertexList.Size() == 0) {
		std::cerr << "ERROR: TriMesh_Make --- vertexList.Size() == 0\n";
		return earlyRet;
	}
	for (unsigned i = 0; i < vertexList.Size(); ++i) {
		if (vertexList[i].GetType() != VariantType::VAR_VECTOR3) {
			std::cerr << "ERROR: TriMesh_Make --- vertexList[i].GetType() != VAR_VECTOR3, i=" << i << "\n";
			return earlyRet;
		}
	}

	if (faceList.Size() == 0) {
		std::cerr << "ERROR: TriMesh_Make --- faceList.Size() == 0\n";
		return earlyRet;
	}
	if (faceList.Size() % 3 != 0) {
		std::cerr << "ERROR: TriMesh_Make --- faceList.Size() % 3 != 0\n";
		return earlyRet;
	}
	for (unsigned i = 0; i < faceList.Size(); ++i) {
		if (faceList[i].GetType() != VariantType::VAR_INT) {
			std::cerr << "ERROR: TriMesh_Make --- faceList[i].GetType() != VAR_INT, i=" << i << "\n";
			return earlyRet;
		}
	}
	int numVertices = (int)vertexList.Size();
	for (int i = 0; i < (int)faceList.Size(); i += 3) {
		int i0 = faceList[i].GetInt();
		int i1 = faceList[i + 1].GetInt();
		int i2 = faceList[i + 2].GetInt();

		if (i0 == i1 || i1 == i2 || i2 == i0) {
			std::cerr << "ERROR: TriMesh_Make --- repeated vertex indices in face\n";
			return earlyRet;
		}

		if (
			(i0 < 0 || i0 > numVertices - 1) ||
			(i1 < 0 || i1 > numVertices - 1) ||
			(i2 < 0 || i2 > numVertices - 1)
			)
		{
			std::cerr << "ERROR: TriMesh_Make --- vertex indices out of range\n";
			return earlyRet;
		}
	}

	VariantMap var_map;
	var_map["type"] = Variant(String("TriMesh"));
	var_map["vertices"] = Variant(vertexList);
	var_map["faces"] = Variant(faceList);
	VariantVector normals = TriMesh_ComputeVertexNormals(var_map);
	var_map["normals"] = Variant(normals);

	return Variant(var_map);
}

Urho3D::Variant TriMesh_Make(const Urho3D::Variant& vertices, const Urho3D::Variant& faces)
{
	Variant earlyRet;
	// extract vertexList
	if (vertices.GetType() != VariantType::VAR_VARIANTVECTOR) {
		std::cerr << "ERROR: TriMesh_Make --- vertices.GetType() != VAR_VARIANTVECTOR\n";
		return earlyRet;
	}
	VariantVector vertexList = vertices.GetVariantVector();
	// extract faceList
	if (faces.GetType() != VariantType::VAR_VARIANTVECTOR) {
		std::cerr << "ERROR: TriMesh_Make --- faces.GetType() != VAR_VARIANTVECTOR\n";
		return earlyRet;
	}
	VariantVector faceList = faces.GetVariantVector();

	return TriMesh_Make(vertexList, faceList);
}

bool TriMesh_Verify(const Urho3D::Variant& triMesh)
{
	if (triMesh.GetType() != VariantType::VAR_VARIANTMAP) return false;

	VariantMap var_map = triMesh.GetVariantMap();
	Variant var_type = var_map["type"];
	if (var_type.GetType() != VariantType::VAR_STRING) return false;

	if (var_type.GetString() != "TriMesh") return false;

	return true;
}

Urho3D::VariantVector TriMesh_GetVertexList(const Urho3D::Variant& triMesh)
{
	bool ver = TriMesh_Verify(triMesh);
	if (!ver) {
		return VariantVector();
	}

	VariantMap var_map = triMesh.GetVariantMap();
	return var_map["vertices"].GetVariantVector();
}
Urho3D::VariantVector TriMesh_GetFaceList(const Urho3D::Variant& triMesh)
{
	bool ver = TriMesh_Verify(triMesh);
	if (!ver) {
		return VariantVector();
	}

	VariantMap var_map = triMesh.GetVariantMap();
	return var_map["faces"].GetVariantVector();
}

Urho3D::VariantVector TriMesh_GetNormalList(const Urho3D::Variant& triMesh)
{
	bool ver = TriMesh_Verify(triMesh);
	if (!ver) {
		return VariantVector();
	}

	VariantMap var_map = triMesh.GetVariantMap();
	return var_map["normals"].GetVariantVector();
}

Urho3D::Vector<float> TriMesh_GetVerticesAsFloats(const Urho3D::Variant& triMesh)
{
	bool ver = TriMesh_Verify(triMesh);
	if (!ver) {
		return Vector<float>();
	}

	VariantVector verts = TriMesh_GetVertexList(triMesh);
	Vector<float> vertsOut;
	int numVerts = verts.Size();
	vertsOut.Resize(numVerts * 3);

	for (int i = 0; i < numVerts; i++)
	{
		Vector3 v = verts[i].GetVector3();
		vertsOut[3 * i] = v.x_;
		vertsOut[3 * i + 1] = v.y_;
		vertsOut[3 * i + 2] = v.z_;
	}

	return vertsOut;
}

Urho3D::Vector<double> TriMesh_GetVerticesAsDoubles(const Urho3D::Variant& triMesh)
{
	bool ver = TriMesh_Verify(triMesh);
	if (!ver) {
		return Vector<double>();
	}

	VariantVector verts = TriMesh_GetVertexList(triMesh);
	Vector<double> vertsOut;
	int numVerts = verts.Size();
	vertsOut.Resize(numVerts * 3);

	for (int i = 0; i < numVerts; i++)
	{
		Vector3 v = verts[i].GetVector3();
		vertsOut[3 * i] = (double)v.x_;
		vertsOut[3 * i + 1] = (double)v.y_;
		vertsOut[3 * i + 2] = (double)v.z_;
	}

	return vertsOut;
}

Urho3D::Vector<int> TriMesh_GetFacesAsInts(const Urho3D::Variant& triMesh)
{
	bool ver = TriMesh_Verify(triMesh);
	if (!ver) {
		return Vector<int>();
	}

	VariantVector faces = TriMesh_GetFaceList(triMesh);
	Vector<int> facesOut;
	int numFaces = faces.Size();
	facesOut.Resize(numFaces);

	for (int i = 0; i < numFaces; i++)
	{
		facesOut[i] = faces[i].GetInt();
	}

	return facesOut;

}

Urho3D::VariantVector TriMesh_ComputeFaceNormals(const Urho3D::Variant& triMesh, bool normalize)
{
	bool ver = TriMesh_Verify(triMesh);
	if (!ver) {
		return VariantVector();
	}

	VariantMap var_map = triMesh.GetVariantMap();
	VariantVector vertexList = var_map["vertices"].GetVariantVector();
	VariantVector faceList = var_map["faces"].GetVariantVector();

	return ComputeFaceNormals(vertexList, faceList, normalize);
}

Urho3D::VariantVector TriMesh_ComputeVertexNormals(const Urho3D::Variant& triMesh, bool normalize)
{
	bool ver = TriMesh_Verify(triMesh);
	if (!ver) {
		return VariantVector();
	}

	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	TriMeshToMatrices(triMesh, V, F);

	VariantVector vertexNormals;
	VariantVector faceNormals = ComputeFaceNormals(V, F);

	std::vector<std::vector<int> > VF;
	std::vector<std::vector<int> > VFi;
	igl::vertex_triangle_adjacency(V, F, VF, VFi);

	// for each vertex index i, compute the normal there
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
		if (normalize) {
			normal.Normalize();
		}

		vertexNormals.Push(normal);
	}

	return vertexNormals;
}

Urho3D::Vector<Pair<int, int>> TriMesh_ComputeEdges(const Urho3D::Variant& triMesh)
{
	bool ver = TriMesh_Verify(triMesh);
	if (!ver) {
		return Vector<Pair<int, int>>();
	}

	//loop through faces and build up the edges
	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	TriMeshToMatrices(triMesh, V, F);

	Eigen::MatrixXi E;
	igl::edges(F, E);

	Vector<Pair<int, int>> edgesOut;
	edgesOut.Resize(E.rows());
	for (int i = 0; i < E.rows(); i++)
	{
		int a = E.row(i)[0];
		int b = E.row(i)[1];

		edgesOut[i] = Pair<int, int>(a, b);
	}

	return edgesOut;
}

Urho3D::Vector<Urho3D::Vector3> TriMesh_ComputePointCloud(const Urho3D::Variant& triMesh)
{
	bool ver = TriMesh_Verify(triMesh);
	if (!ver) {
		return Vector<Vector3>();
	}

	Vector<Vector3> point_cloud;
	VariantVector vertex_list = TriMesh_GetVertexList(triMesh);

	for (unsigned i = 0; i < vertex_list.Size(); ++i) {
		point_cloud.Push(vertex_list[i].GetVector3());
	}

	return point_cloud;
}

Urho3D::Variant TriMesh_ApplyTransform(
	const Urho3D::Variant& tri_mesh,
	const Urho3D::Matrix3x4& T
	)
{
	VariantVector vertex_list = TriMesh_GetVertexList(tri_mesh);
	VariantVector face_list = TriMesh_GetFaceList(tri_mesh);

	VariantVector transformed_vertex_list = Geomlib::TransformVertexList(T, vertex_list);

	return TriMesh_Make(transformed_vertex_list, face_list);
}

Urho3D::Variant TriMesh_CullUnusedVertices(
	const Urho3D::Variant& tri_mesh
)
{
	Eigen::MatrixXf V, NV;
	Eigen::MatrixXi F, NF;
	bool success = IglMeshToMatrices(tri_mesh, V, F);
	if (!success) {
		return Variant();
	}

	Eigen::VectorXi _1;
	igl::remove_unreferenced(V, F, NV, NF, _1);
	Variant out_mesh = TriMesh_Make(NV, NF);

	return out_mesh;
}

Urho3D::Variant TriMesh_DoubleAndFlipFaces(
	const Urho3D::Variant& tri_mesh
	)
{
	VariantVector verts = TriMesh_GetVertexList(tri_mesh);
	VariantVector faces = TriMesh_GetFaceList(tri_mesh);
	VariantVector newFaces;
	int numFaces = faces.Size() / 3;
	for (int i = 0; i < numFaces; i++)
	{
		int a = faces[3 * i].GetInt();
		int b = faces[3 * i + 1].GetInt();
		int c = faces[3 * i + 2].GetInt();

		newFaces.Push(a);
		newFaces.Push(c);
		newFaces.Push(b);
	}

	faces.Insert(numFaces, newFaces);

	return TriMesh_Make(verts, faces);
}

Urho3D::Variant TriMesh_BoundingBox(const Urho3D::Variant& tri_mesh)
{
	if (!TriMesh_Verify(tri_mesh)) {
		return Variant();
	}

	Eigen::MatrixXf V, BV;
	Eigen::MatrixXi F, BF;
	IglMeshToMatrices(tri_mesh, V, F);
	igl::bounding_box(V, BV, BF);

	return TriMesh_Make(BV, BF);
}

Urho3D::Variant TriMesh_UnifyNormals(const Urho3D::Variant& tri_mesh)
{
	if (!TriMesh_Verify(tri_mesh)) {
		return Variant();
	}

	Eigen::MatrixXf V;
	Eigen::MatrixXi F, FF;
	Eigen::VectorXi C;
	IglMeshToMatrices(tri_mesh, V, F);
	igl::bfs_orient(F, FF, C);

	return TriMesh_Make(V, FF);
}

void TriMeshToMatrices(const Variant& triMesh, Eigen::MatrixXf& V, Eigen::MatrixXi& F)
{
	VariantMap var_map = triMesh.GetVariantMap();
	VariantVector vertexList = var_map["vertices"].GetVariantVector();
	VariantVector faceList = var_map["faces"].GetVariantVector();

	unsigned numVertices = vertexList.Size();
	V.setZero(numVertices, 3);
	for (unsigned i = 0; i < numVertices; ++i) {
		Vector3 vert = vertexList[i].GetVector3();
		V.row(i) = Eigen::RowVector3f(vert.x_, vert.y_, vert.z_);
	}

	unsigned numFaces = faceList.Size() / 3;
	F.setZero(numFaces, 3);
	for (unsigned i = 0; i < numFaces; ++i) {
		for (unsigned j = 0; j < 3; ++j) {
			F(i, j) = faceList[3 * i + j].GetInt();
		}
	}
}

void TriMeshToDoubleMatrices(const Variant& triMesh, Eigen::MatrixXd& V, Eigen::MatrixXi& F)
{
	VariantMap var_map = triMesh.GetVariantMap();
	VariantVector vertexList = var_map["vertices"].GetVariantVector();
	VariantVector faceList = var_map["faces"].GetVariantVector();

	unsigned numVertices = vertexList.Size();
	V.setZero(numVertices, 3);
	for (unsigned i = 0; i < numVertices; ++i) {
		Vector3 vert = vertexList[i].GetVector3();
		V.row(i) = Eigen::RowVector3d(vert.x_, vert.y_, vert.z_);
	}

	unsigned numFaces = faceList.Size() / 3;
	F.setZero(numFaces, 3);
	for (unsigned i = 0; i < numFaces; ++i) {
		for (unsigned j = 0; j < 3; ++j) {
			F(i, j) = faceList[3 * i + j].GetInt();
		}
	}
}

////////////////////////////////////////////////////////////////////////////
/////////////////////////// DISPLAY ////////////////////////////////////////

//vertex data types

#include <Urho3D/Graphics/VertexBuffer.h>
#include <Urho3D/Graphics/IndexBuffer.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Geometry.h>

struct VertexData
{
	Vector3 position; // size of 12 bytes +
	Vector3 normal; // size of 12 bytes +
	unsigned color;
};

Urho3D::Model* TriMesh_GetRenderMesh(const Urho3D::Variant& triMesh, Urho3D::Context* context, VariantVector vColors, bool split)
{
	if (!TriMesh_Verify(triMesh))
	{
		return NULL;
	}

	Vector<VertexData> vbd;
	Vector<Vector3> tmpVerts;
	Vector<int> tmpFaces;

	VariantVector verts = TriMesh_GetVertexList(triMesh);
	VariantVector faces = TriMesh_GetFaceList(triMesh);
	VariantVector normals;

	if (vColors.Empty())
	{
		vColors.Push(Color::WHITE);
	}
	
	int numColors = vColors.Size();

	if (split)
	{
		normals = TriMesh_ComputeFaceNormals(triMesh, true);
		tmpFaces.Resize(faces.Size());
		tmpVerts.Resize(faces.Size());
		vbd.Resize(faces.Size());

		unsigned int vCol = Urho3D::Color::CYAN.ToUInt();

		//render with duplicate verts for flat face shading
		for (unsigned i = 0; i < faces.Size(); i++)
		{
			int fId = faces[i].GetInt();
			int normId = (int)floor((float)i / 3.0f);
			unsigned int col = vColors[normId%numColors].GetColor().ToUInt();
			if (fId < (int)verts.Size())
			{
				vbd[i].position = verts[fId].GetVector3();
				vbd[i].normal = normals[normId].GetVector3();
				vbd[i].color = col;
				tmpVerts[i] = vbd[i].position;
			}

			tmpFaces[i] = i;

		}
	}
	else
	{
		normals = TriMesh_ComputeVertexNormals(triMesh);
		tmpFaces.Resize(faces.Size());
		tmpVerts.Resize(verts.Size());
		vbd.Resize(verts.Size());
		unsigned int vCol = Color::CYAN.ToUInt();

		for (unsigned i = 0; i < verts.Size(); i++)
		{
			vbd[i].position = verts[i].GetVector3();
			vbd[i].normal = normals[i].GetVector3();
			vbd[i].color = vColors[i%numColors].GetColor().ToUInt();
			tmpVerts[i] = verts[i].GetVector3();
		}

		for (unsigned i = 0; i < faces.Size(); i++)
		{
			tmpFaces[i] = faces[i].GetInt();
		}
	}

	SharedPtr<VertexBuffer> vb(new VertexBuffer(context));
	SharedPtr<IndexBuffer> ib(new IndexBuffer(context));

	// Shadowed buffer needed for raycasts to work, and so that data can be automatically restored on device loss
	vb->SetShadowed(true);
	vb->SetSize(vbd.Size(), Urho3D::MASK_POSITION | Urho3D::MASK_NORMAL | Urho3D::MASK_COLOR);
	vb->SetData((void*)&vbd[0]);

	ib->SetShadowed(true);
	ib->SetSize(tmpFaces.Size(), true);
	ib->SetData(&tmpFaces[0]);

	Geometry* geom = new Geometry(context);
	geom->SetNumVertexBuffers(1);
	geom->SetVertexBuffer(0, vb);
	geom->SetIndexBuffer(ib);
	geom->SetDrawRange(Urho3D::TRIANGLE_LIST, 0, faces.Size());

	Model* model = new Model(context);
	model->SetNumGeometries(1);
	model->SetGeometry(0, 0, geom);
	model->SetBoundingBox(BoundingBox(&tmpVerts[0], tmpVerts.Size()));
	model->SetGeometryCenter(0, Vector3::ZERO);

	Vector<SharedPtr<VertexBuffer>> allVBuffers;
	Vector<SharedPtr<IndexBuffer>> allIBuffers;

	allVBuffers.Push(vb);
	allIBuffers.Push(ib);

	PODVector<unsigned int> morphStarts;
	PODVector<unsigned int> morphRanges;

	model->SetVertexBuffers(allVBuffers, morphStarts, morphRanges);
	model->SetIndexBuffers(allIBuffers);

	return model;
}