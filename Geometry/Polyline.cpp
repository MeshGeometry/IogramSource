#include "Polyline.h"

#include <iostream>

#include "Geomlib_RemoveDuplicates.h"
#include "Geomlib_PolylineOffset.h"
#include "Geomlib_PolylineLoft.h"
#include "Geomlib_TriMeshThicken.h"
#include "Geomlib_TransformVertexList.h"
#include "TriMesh.h"

using Urho3D::Color;
using Urho3D::Equals;
using Urho3D::Model;
using Urho3D::SharedPtr;
using Urho3D::String;
using Urho3D::Variant;
using Urho3D::VariantMap;
using Urho3D::VariantType;
using Urho3D::VariantVector;
using Urho3D::Vector;
using Urho3D::Vector3;
using Urho3D::Pair;

namespace {

	bool Vector3Equals(const Vector3& lhs, const Vector3& rhs)
	{
		return Equals(lhs.x_, rhs.x_) && Equals(lhs.y_, rhs.y_) && Equals(lhs.z_, rhs.z_);
	}

} // namespace

Urho3D::Variant Polyline_Make(const Urho3D::VariantVector& vertexList)
{
	Variant earlyRet;
	if (vertexList.Size() == 0) {
		std::cerr << "ERROR: Polyline_Make --- vertexList.Size() == 0\n";
		return earlyRet;
	}
	for (unsigned i = 0; i < vertexList.Size(); ++i) {
		if (vertexList[i].GetType() != VariantType::VAR_VECTOR3) {
			std::cerr << "ERROR: Polyline_Make --- vertexList[i].GetType() != VAR_VECTOR3, i=" << i << "\n";
			return earlyRet;
		}
	}

	Variant vertices;
	Variant edges;
	Geomlib::RemoveDuplicates(vertexList, vertices, edges);

	VariantMap var_map;
	var_map["type"] = Variant(String("Polyline"));
	var_map["vertices"] = vertices;
	var_map["edges"] = edges;

	return Variant(var_map);
}

bool Polyline_Verify(const Urho3D::Variant& polyline)
{
	if (polyline.GetType() != VariantType::VAR_VARIANTMAP) return false;

	VariantMap var_map = polyline.GetVariantMap();
	Variant var_type = var_map["type"];
	if (var_type.GetType() != VariantType::VAR_STRING) return false;

	if (var_type.GetString() != "Polyline") return false;

	return true;
}

bool Polyline_IsClosed(const Urho3D::Variant& polyline)
{
	if (!Polyline_Verify(polyline)) return false;

	VariantMap var_map = polyline.GetVariantMap();
	VariantVector edgeList = var_map["edges"].GetVariantVector();
	int numEdges = edgeList.Size();
	if (numEdges < 3)
		return false;

	if (edgeList[0].GetInt() == edgeList[numEdges - 1].GetInt())
	{
		return true;
	}
	else
		return false;
}

VariantVector Polyline_GetVertexList(const Urho3D::Variant& polyline)
{
	if (!Polyline_Verify(polyline)) return VariantVector();

	VariantMap var_map = polyline.GetVariantMap();
	return var_map["vertices"].GetVariantVector();
}

VariantVector Polyline_ComputeSequentialVertexList(const Urho3D::Variant& polyline)
{
	if (!Polyline_Verify(polyline)) return VariantVector();

	VariantMap var_map = polyline.GetVariantMap();
	VariantVector vertexList = var_map["vertices"].GetVariantVector();
	VariantVector edgeList = var_map["edges"].GetVariantVector();

	VariantVector seqVertexList;
	for (unsigned i = 0; i < edgeList.Size(); ++i) {
		unsigned j = edgeList[i].GetInt();
		seqVertexList.Push(vertexList[j].GetVector3());
	}

	return seqVertexList;
}

Urho3D::Vector<Urho3D::Pair<int, int>> Polyline_ComputeEdges(const Urho3D::Variant& polyline)
{
	if (!Polyline_Verify(polyline)) return Vector<Urho3D::Pair<int, int>>();

	VariantMap var_map = polyline.GetVariantMap();
	VariantVector edgeList = var_map["edges"].GetVariantVector();
	Vector<Pair<int, int>> edgesOut;
	for (unsigned i = 0; i < edgeList.Size() - 1; i++)
	{
		Pair<int, int> edge(edgeList[i].GetInt(), edgeList[i + 1].GetInt());
		edgesOut.Push(edge);

	}

	return edgesOut;
}

int Polyline_GetSequentialVertexNumber(const Urho3D::Variant& polyline)
{
	if (!Polyline_Verify(polyline)) return -1;

	VariantMap var_map = polyline.GetVariantMap();
	VariantVector edgeList = var_map["edges"].GetVariantVector();

	return (int)edgeList.Size();
}

Urho3D::Variant Polyline_Clean(const Urho3D::Variant& polyline)
{
	if (!Polyline_Verify(polyline)) return Variant();

	VariantVector seqVertexList = Polyline_ComputeSequentialVertexList(polyline);
	if (seqVertexList.Size() == 0) {
		std::cerr << "ERROR: Polyline_Clean --- seqVertexList.Size()\n";
		return Variant();
	}

	VariantVector cleanVertexList;
	cleanVertexList.Push(seqVertexList[0]);
	for (unsigned i = 1; i < seqVertexList.Size(); ++i) {
		Vector3 prev = seqVertexList[i - 1].GetVector3();
		Vector3 curr = seqVertexList[i].GetVector3();
		if (!Vector3Equals(prev, curr)) {
			cleanVertexList.Push(curr);
		}
	}

	if (cleanVertexList.Size() == 0) {
		std::cerr << "ERROR: Polyline_Clean --- cleanVertexList.Size() == 0\n";
		return Variant();
	}

	return Polyline_Make(cleanVertexList);
}

Urho3D::Vector<Urho3D::Vector3> Polyline_ComputePointCloud(const Urho3D::Variant& polyline)
{
	bool ver = Polyline_Verify(polyline);
	if (!ver) {
		return Vector<Vector3>();
	}

	Vector<Vector3> point_cloud;
	VariantVector vertex_list = Polyline_GetVertexList(polyline);

	for (unsigned i = 0; i < vertex_list.Size(); ++i) {
		point_cloud.Push(vertex_list[i].GetVector3());
	}

	return point_cloud;
}

Urho3D::Variant Polyline_ApplyTransform(
	const Urho3D::Variant& polyline,
	const Urho3D::Matrix3x4& T
)
{
	VariantVector vertex_list = Polyline_ComputeSequentialVertexList(polyline);
	VariantVector transformed_vertex_list = Geomlib::TransformVertexList(T, vertex_list);

	return Polyline_Make(transformed_vertex_list);
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


Urho3D::SharedPtr<Model> Polyline_GetRenderMesh(const Urho3D::Variant& poly, Urho3D::Context* context, Urho3D::VariantVector vCols, float thickness, bool split)
{
	SharedPtr<Model> model(new Model(context));

	if (!Polyline_Verify(poly))
	{
		return model;
	}

	Variant polyA;
	Geomlib::PolylineOffset(poly, polyA, 0.5f * thickness);
	Variant polyB;
	Geomlib::PolylineOffset(poly, polyB, -0.5f * thickness);

	Variant polyMesh;
	Vector<Variant> polys;
	polys.Push(polyA);
	polys.Push(polyB);
	Geomlib::PolylineLoft(polys, polyMesh);
	Variant thickMesh;
	Geomlib::TriMeshThicken(polyMesh, 0.0001f, thickMesh);
	//Variant backFaceMesh = TriMesh_DoubleAndFlipFaces(polyMesh);
	VariantVector doubledCols;
	if (vCols.Empty())
	{
		doubledCols.Push(Color(0.8f, 0.8f, 1.0f, 1.0f));
	}
	else
	{
		
		for (unsigned i = 0; i < vCols.Size(); i++)
		{
			doubledCols.Push(vCols[i]);
			doubledCols.Push(vCols[i]);
			doubledCols.Push(vCols[i]);
			doubledCols.Push(vCols[i]);
		}
	}

	model = TriMesh_GetRenderMesh(thickMesh, context, doubledCols, split);

	return model;
}