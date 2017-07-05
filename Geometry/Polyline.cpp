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


#include "Polyline.h"

#include <iostream>

#include "Geomlib_RemoveDuplicates.h"
#include "Geomlib_PolylineOffset.h"
#include "Geomlib_PolylineLoft.h"
#include "Geomlib_TriMeshThicken.h"
#include "Geomlib_TransformVertexList.h"
#include "TriMesh.h"

#define CHECK_GEO_REG(result) if (result <= 0) { \
		printf("geo_reg: FAIL\n"); \
		failed = true; \
	}

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

using namespace Urho3D;

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

Urho3D::Variant Polyline_Make(const Urho3D::Vector<Urho3D::Vector3>& vertexList)
{
	Variant earlyRet;
	if (vertexList.Size() < 2) {
		std::cerr << "ERROR: Polyline_Make --- vertexList.Size() == 0\n";
		return earlyRet;
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



Urho3D::Variant Polyline_Make(const Urho3D::VariantVector& vertexList, const Urho3D::VariantVector& edgeList)
{
	Variant earlyRet;
	if (vertexList.Size() == 0) {
		std::cout << "ERROR: Polyline_Make --- vertexList.Size() == 0" << std::endl;
		return earlyRet;
	}
	for (unsigned i = 0; i < vertexList.Size(); ++i) {
		if (vertexList[i].GetType() != VariantType::VAR_VECTOR3) {
			std::cout << "ERROR: Polyline_Make --- vertexList[i].GetType() != VAR_VECTOR3, i=" << i << std::endl;
			return earlyRet;
		}
	}

	if (edgeList.Size() == 0) {
		std::cout << "ERROR: Polyline_Make --- faceList.Size() == 0" << std::endl;
		return earlyRet;
	}
	for (unsigned i = 0; i < edgeList.Size(); ++i) {
		if (edgeList[i].GetType() != VariantType::VAR_INT) {
			std::cout << "ERROR: Polyline_Make --- faceList[i].GetType() != VAR_INT, i=" << i << std::endl;
			return earlyRet;
		}
	}
	int numVertices = (int)vertexList.Size();
	for (int i = 0; i < (int)edgeList.Size(); ++i) {
		//
	}
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

void Polyline_Close(Urho3D::Variant& polyline)
{
	if (!Polyline_Verify(polyline)) return;

	if (!Polyline_IsClosed(polyline))
	{
		
		VariantVector vertices = Polyline_ComputeSequentialVertexList(polyline);

		//duplicate last vertex
		vertices.Push(vertices[0]);
		polyline = Polyline_Make(vertices);
	}

	return;

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

float Polyline_GetCurveLength(const Urho3D::Variant& polyline)
{
	if (!Polyline_Verify(polyline)) return 0.0f;
	
	VariantVector verts = Polyline_ComputeSequentialVertexList(polyline);
	float length = 0.0f;

	if (verts.Size() < 2)
		return 0.0f;

	for (int i = 0; i < verts.Size() - 1; i++)
	{
		Vector3 vA = verts[i].GetVector3();
		Vector3 vB = verts[i + 1].GetVector3();

		length += (vB - vA).Length();
	}

	return length;
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

Urho3D::Variant Polyline_Rebuild(const Urho3D::Variant& polyline, int numTargetPoints)
{
	bool ver = Polyline_Verify(polyline);
	if (!ver) {
		return Variant();
	}
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

// script versions

Urho3D::Variant Polyline_MakeFromVariantArray(CScriptArray* vertexList_arr)
{
	Vector<Variant> vertexList = ArrayToVector<Variant>(vertexList_arr);
	return Polyline_Make(vertexList);
}

Urho3D::Variant Polyline_MakeFromVector3Array(CScriptArray* vertexList_arr)
{
	Vector<Vector3> vertexList = ArrayToVector<Vector3>(vertexList_arr);
	return Polyline_Make(vertexList);
}

Urho3D::CScriptArray* Polyline_GetVertexArray(const Urho3D::Variant& polyline)
{
	VariantVector vertexList = Polyline_GetVertexList(polyline);
	return VectorToArray<Variant>(vertexList, "Array<Variant>");
}

Urho3D::CScriptArray* Polyline_ComputeSequentialVertexArray(const Urho3D::Variant& polyline)
{
	VariantVector vertexList = Polyline_ComputeSequentialVertexList(polyline);
	return VectorToArray<Variant>(vertexList, "Array<Variant>");
}

Urho3D::CScriptArray* Polyline_ComputePointCloudArray(const Urho3D::Variant& polyline)
{
	Vector<Vector3> cloud = Polyline_ComputePointCloud(polyline);
	return VectorToArray<Vector3>(cloud, "Array<Vector3>");
}

////////////////////////////////////////////
// Register polyline functions for scripting

bool RegisterPolylineFunctions(Urho3D::Context* context)
{
	bool failed = false;

	Script* script_system = context->GetSubsystem<Script>();
	asIScriptEngine* engine = script_system->GetScriptEngine();
	
	int res;

	res = engine->RegisterGlobalFunction(
		"Variant Polyline_MakeFromVariantArray(Array<Variant>@)",
		asFUNCTION(Polyline_MakeFromVariantArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

		res = engine->RegisterGlobalFunction(
			"Variant Polyline_MakeFromVector3Array(Array<Vector3>@)",
			asFUNCTION(Polyline_MakeFromVector3Array),
			asCALL_CDECL
		);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool Polyline_Verify(const Variant&)",
		asFUNCTION(Polyline_Verify),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"void Polyline_Close(Variant&)",
		asFUNCTION(Polyline_Close),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Array<Variant>@ Polyline_GetVertexArray(const Variant&)",
		asFUNCTION(Polyline_GetVertexArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"int Polyline_GetSequentialVertexNumber(const Variant&)",
		asFUNCTION(Polyline_GetSequentialVertexNumber),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Array<Variant>@ Polyline_ComputeSequentialVertexArray(const Variant&)",
		asFUNCTION(Polyline_ComputeSequentialVertexArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"float Polyline_GetCurveLength(const Variant&)",
		asFUNCTION(Polyline_GetCurveLength),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Variant Polyline_Clean(const Variant&)",
		asFUNCTION(Polyline_Clean),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Array<Vector3>@ Polyline_ComputePointCloudArray(const Variant&)",
		asFUNCTION(Polyline_ComputePointCloudArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	//TODO: Polyline_ComputeEdges

	res = engine->RegisterGlobalFunction(
		"bool Polyline_IsClosed(const Variant&)",
		asFUNCTION(Polyline_IsClosed),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Variant Polyline_ApplyTransform(const Variant&, const Matrix3x4&)",
		asFUNCTION(Polyline_ApplyTransform),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	//TODO: Polyline_GetRenderMesh

	if (failed) {
		URHO3D_LOGINFO("RegisterPolylineFunctions --- Failed to compile scripts");
	}
	else {
		URHO3D_LOGINFO("RegisterPolylineFunctions --- OK!");
	}

	return !failed;
}