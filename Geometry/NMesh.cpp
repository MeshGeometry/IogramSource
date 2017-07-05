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


#include "NMesh.h"

#include <iostream>

#include "TriMesh.h"
#include "Polyline.h"
#include "Geomlib_TriangulatePolygon.h"

#include <Urho3D/Math/MathDefs.h>

#define CHECK_GEO_REG(result) if (result <= 0) { \
		printf("geo_reg: FAIL\n"); \
		failed = true; \
	}

using Urho3D::String;
using Urho3D::Variant;
using Urho3D::VariantMap;
using Urho3D::VariantType;
using Urho3D::VariantVector;
using Urho3D::Vector3;
using Urho3D::Vector;
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

Variant Face_Make(const VariantVector& vertex_indices)
{
	VariantMap map;
	map["face_vertices"] = Variant(vertex_indices);

	return Variant(map);
}

Variant Face_MakePolyline(const Variant& face, const VariantVector& global_vertex_list)
{
	VariantMap map = face.GetVariantMap();
	VariantVector vertex_indices = map["face_vertices"].GetVariantVector();

	VariantVector poly_vertex_list;
	for (unsigned i = 0; i < vertex_indices.Size(); ++i) {
		int j = vertex_indices[i].GetInt();
		poly_vertex_list.Push(global_vertex_list[j].GetVector3());
	}
	int j = vertex_indices[0].GetInt();
	poly_vertex_list.Push(global_vertex_list[j].GetVector3());

	return Polyline_Make(poly_vertex_list);
}

void AddTris(const Variant& face, VariantVector& tri_list)
{
	VariantMap var_map = face.GetVariantMap();
	VariantVector verts = var_map["face_vertices"].GetVariantVector();

	int v0 = verts[0].GetInt();
	for (unsigned i = 1; i < verts.Size() - 1; ++i) {
		int v1 = verts[i].GetInt();
		int v2 = verts[i + 1].GetInt();
		tri_list.Push(Variant(v0));
		tri_list.Push(Variant(v1));
		tri_list.Push(Variant(v2));
	}
}

} // namespace

Urho3D::Variant NMesh_Make(const Urho3D::VariantVector& vertex_list, const Urho3D::VariantVector& face_list)
{
	Variant early_ret;
	if (vertex_list.Size() == 0) {
		std::cerr << "ERROR: NMesh_Make --- vertex_list.size() == 0\n";
		return early_ret;
	}
	for (unsigned i = 0; i < vertex_list.Size(); ++i) {
		if (vertex_list[i].GetType() != VariantType::VAR_VECTOR3) {
			std::cerr << "ERROR: NMesh_Make --- vertex_list[i].GetType() != VAR_VECTOR3, i=" << i << "\n";
			return early_ret;
		}
	}

	if (face_list.Size() == 0) {
		std::cerr << "ERROR: NMesh_Make --- face_list.Size() == 0\n";
		return early_ret;
	}
	for (unsigned i = 0; i < face_list.Size(); ++i) {
		if (face_list[i].GetType() != VariantType::VAR_INT) {
			std::cerr << "ERROR: NMesh_Make --- face_list[i].GetType() != VAR_INT, i=" << i << "\n";
			return early_ret;
		}
	}

	VariantVector structured_face_list;
	for (unsigned i = 0; i < face_list.Size(); ++i) {
		int num_verts_in_face = face_list[i].GetInt();

		// No combinatorially degenerate faces!
		if (num_verts_in_face <= 2) {
			std::cerr << "ERROR: NMesh_Make --- num_verts_in_face <=2\n";
			return early_ret;
		}

		VariantVector indices_for_this_face;
		for (int j = 0; j < num_verts_in_face; ++j) {
			int i0 = i + j + 1;
			if (!(i0 < (int)face_list.Size())) {
				std::cerr << "ERROR: NMesh_Make --- !(i0 < face_list.Size())\n";
				return early_ret;
			}
			int vertex_index = face_list[i0].GetInt();
			indices_for_this_face.Push(Variant(vertex_index));
		}
		i += num_verts_in_face;

		// ALERT: still need to check consistency of vertex indices
		Variant this_face = Face_Make(indices_for_this_face);
		structured_face_list.Push(this_face);
	}

	VariantMap var_map;
	var_map["type"] = Variant(String("NMesh"));
	var_map["vertices"] = Variant(vertex_list);
	var_map["faces"] = Variant(structured_face_list);

	return Variant(var_map);
}

bool NMesh_Verify(const Urho3D::Variant& nmesh)
{
	if (nmesh.GetType() != VariantType::VAR_VARIANTMAP) return false;

	VariantMap var_map = nmesh.GetVariantMap();
	Variant var_type = var_map["type"];
	if (var_type.GetType() != VariantType::VAR_STRING) return false;

	if (var_type.GetString() != "NMesh") return false;

	return true;
}

Urho3D::VariantVector NMesh_GetVertexList(const Urho3D::Variant& nmesh)
{
	if (!NMesh_Verify(nmesh)) {
		return VariantVector();
	}

	VariantMap var_map = nmesh.GetVariantMap();
	return var_map["vertices"].GetVariantVector();
}

Urho3D::VariantVector NMesh_GetFaceList(const Urho3D::Variant& nmesh)
{
	if (!NMesh_Verify(nmesh)) {
		return VariantVector();
	}

	VariantMap var_map = nmesh.GetVariantMap();
	return var_map["faces"].GetVariantVector();
}

Urho3D::Variant NMesh_ConvertToTriMesh(const Urho3D::Variant& nmesh)
{
	if (!NMesh_Verify(nmesh)) {
		return Variant();
	}

	VariantMap nmesh_map = nmesh.GetVariantMap();
	VariantVector vertex_list = nmesh_map["vertices"].GetVariantVector();
	VariantVector structured_face_list = nmesh_map["faces"].GetVariantVector();

	VariantVector tri_list;
	for (unsigned i = 0; i < structured_face_list.Size(); ++i) {
		AddTris(structured_face_list[i], tri_list);
	}

	Variant tri_mesh = TriMesh_Make(vertex_list, tri_list);

	return tri_mesh;
}

Urho3D::Variant NMesh_ConvertToTriMesh(const Urho3D::Variant& nmesh, Urho3D::VariantVector& faceTris)
{
    if (!NMesh_Verify(nmesh)) {
        return Variant();
    }
    
    VariantMap nmesh_map = nmesh.GetVariantMap();
    VariantVector vertex_list = nmesh_map["vertices"].GetVariantVector();
    VariantVector structured_face_list = nmesh_map["faces"].GetVariantVector();
    
    VariantVector tri_list;
    for (unsigned i = 0; i < structured_face_list.Size(); ++i) {
        int startID = tri_list.Size();
        AddTris(structured_face_list[i], tri_list);
        Urho3D::VariantVector currFaceTris;
        for (int j = startID; j < tri_list.Size(); ++j){
            currFaceTris.Push(tri_list[j].GetInt());
        }
        faceTris.Push(Variant(currFaceTris));
    }
    
    Variant tri_mesh = TriMesh_Make(vertex_list, tri_list);
    
    return tri_mesh;
}

// faceTris is a vector<vector<int>> that keeps track of what triangles belong to what ngon face
// It is effectively a vector of the face lists for each triangulated polygon 
Urho3D::Variant NMesh_ConvertToTriMesh_P2T(const Urho3D::Variant& nmesh, Urho3D::VariantVector& faceTris)
{
	if (!NMesh_Verify(nmesh)) {
		return Variant();
	}

	VariantMap nmesh_map = nmesh.GetVariantMap();
	VariantVector vertex_list = nmesh_map["vertices"].GetVariantVector();
	VariantVector structured_face_list = nmesh_map["faces"].GetVariantVector();

	VariantVector tri_list;
	for (unsigned i = 0; i < structured_face_list.Size(); ++i) {
        int startID = tri_list.Size();
		Geomlib::TriangulateFace(nmesh, i, tri_list);
        Urho3D::VariantVector currFaceTris;
        for (int j = startID; j < tri_list.Size(); ++j){
            currFaceTris.Push(tri_list[j].GetInt());
        }
        faceTris.Push(Variant(currFaceTris));
	}

	Variant tri_mesh = TriMesh_Make(vertex_list, tri_list);

	return tri_mesh;
}

Urho3D::Vector<Urho3D::Variant> NMesh_ComputeWireframePolylines(const Urho3D::Variant& nmesh)
{
	if (!NMesh_Verify(nmesh)) {
		return Vector<Variant>();
	}

	VariantVector vertex_list = NMesh_GetVertexList(nmesh);
	VariantVector structured_face_list = NMesh_GetFaceList(nmesh);

	Vector<Variant> face_polylines;

	for (unsigned i = 0; i < structured_face_list.Size(); ++i) {
		Variant face = structured_face_list[i];
		Variant cur_poly = Face_MakePolyline(face, vertex_list);
		face_polylines.Push(cur_poly);
	}

	return face_polylines;
}

Urho3D::Variant NMesh_GetFacePolyline(const Urho3D::Variant& nmesh, int face_ID, Urho3D::VariantVector& face_indices)
{
	Urho3D::VariantVector structured_face_list = NMesh_GetFaceList(nmesh);
	Urho3D::VariantVector vertex_list = NMesh_GetVertexList(nmesh);

	VariantMap face_indices_map = structured_face_list[face_ID].GetVariantMap();
	face_indices = face_indices_map["face_vertices"].GetVariantVector();


	VariantVector faceVertices;
	for (int i = 0; i < face_indices.Size(); ++i)
	{
		int curID = face_indices[i].GetInt();
		Vector3 curVert = vertex_list[curID].GetVector3();
		faceVertices.Push(Variant(curVert));
	}

	Variant face_polyline = Polyline_Make(faceVertices);

	return face_polyline;
}

Urho3D::Vector<Pair<int, int>> NMesh_ComputeEdges(const Urho3D::Variant& NMesh)
{
    bool ver = NMesh_Verify(NMesh);
    if (!ver) {
        return Vector<Pair<int, int>>();
    }
    
    Urho3D::VariantVector structured_face_list = NMesh_GetFaceList(NMesh);
    Urho3D::VariantVector vertex_list = NMesh_GetVertexList(NMesh);
    
    Vector<Pair<int, int>> edgesOut;
    
    for (unsigned i = 0; i < structured_face_list.Size(); ++i) {
        VariantMap face = structured_face_list[i].GetVariantMap();
        Urho3D::VariantVector face_indices = face["face_vertices"].GetVariantVector();
        for (int j = 0; j < face_indices.Size(); ++j)
        {
            int curID = face_indices[j].GetInt();
            int nextID = face_indices[(j+1) % face_indices.Size()].GetInt();
            
            edgesOut.Push(Pair<int, int>(curID, nextID));
        }
    }
    
    return edgesOut;
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

Urho3D::Model* NMesh_GetRenderMesh(const Urho3D::Variant& nMesh, Urho3D::Context* context, VariantVector vColors, bool split)
{
    if (!NMesh_Verify(nMesh))
    {
        return NULL;
    }
    
    Vector<VertexData> vbd;
    Vector<Vector3> tmpVerts;
    Vector<int> tmpFaces;
    
    VariantVector verts = NMesh_GetVertexList(nMesh);
    VariantVector faces = NMesh_GetFaceList(nMesh);
    VariantVector normals;
    
    if (vColors.Empty())
    {
        vColors.Push(Color::WHITE);
    }
    
    int numColors = vColors.Size();
    
    // want to render each ngon face with flat shading.
    
    Urho3D::VariantVector faceTris;
    Urho3D::Variant convertedMesh = NMesh_ConvertToTriMesh_P2T(nMesh, faceTris);
	Urho3D::Variant unifiedMesh = TriMesh_UnifyNormals(convertedMesh);
    
    Model* model = new Model(context);
    model = TriMesh_GetRenderMesh(unifiedMesh, context, vColors, true);
    

    return model;
}

// for scripts
Urho3D::Variant NMesh_MakeFromVariantArrays(Urho3D::CScriptArray* vertex_array, Urho3D::CScriptArray* face_array)
{
	Vector<Variant> vertex_list = Urho3D::ArrayToVector<Variant>(vertex_array);
	Vector<Variant> face_list = Urho3D::ArrayToVector<Variant>(face_array);
	return NMesh_Make(vertex_list, face_list);
}

Urho3D::CScriptArray* NMesh_GetVertexArray(const Urho3D::Variant& nmesh)
{
	Vector<Variant> vertex_list = NMesh_GetVertexList(nmesh);
	return Urho3D::VectorToArray<Variant>(vertex_list, "Array<Variant>");
}

Urho3D::CScriptArray* NMesh_GetFaceArray(const Urho3D::Variant& nmesh)
{
	Vector<Variant> face_list = NMesh_GetFaceList(nmesh);
	return Urho3D::VectorToArray<Variant>(face_list, "Array<Variant>");
}

Urho3D::Variant NMesh_ConvertToTriMeshFromVariant(const Urho3D::Variant& nmesh)
{
	return NMesh_ConvertToTriMesh(nmesh);
}

Urho3D::Variant NMesh_ConvertToTriMeshFromVariantAndFaceTris(const Urho3D::Variant& nmesh, Urho3D::CScriptArray* face_tris_array)
{
	Vector<Variant> face_tris_list = Urho3D::ArrayToVector<Variant>(face_tris_array);
	return NMesh_ConvertToTriMesh(nmesh, face_tris_list);
}

Urho3D::CScriptArray* NMesh_ComputeWireframePolylinesArray(const Urho3D::Variant& nmesh)
{
	Vector<Variant> polyline_list = NMesh_ComputeWireframePolylines(nmesh);
	return Urho3D::VectorToArray<Variant>(polyline_list, "Array<Variant>");
}

Urho3D::Variant NMesh_GetFacePolylineFromIndicesArray(const Urho3D::Variant& nmesh, int face_ID, Urho3D::CScriptArray* face_indices_arr)
{
	Vector<Variant> face_indices = Urho3D::ArrayToVector<Variant>(face_indices_arr);
	return NMesh_GetFacePolyline(nmesh, face_ID, face_indices);
}

bool RegisterNMeshFunctions(Urho3D::Context* context)
{
	Urho3D::Script* script_system = context->GetSubsystem<Urho3D::Script>();
	asIScriptEngine* engine = script_system->GetScriptEngine();

	bool failed = false;

	int res;

	res = engine->RegisterGlobalFunction(
		"Variant NMesh_MakeFromVariantArrays(Array<Variant>@, Array<Variant>@)",
		asFUNCTION(NMesh_MakeFromVariantArrays),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"bool NMesh_Verify(const Variant&)",
		asFUNCTION(NMesh_Verify),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Array<Variant>@ NMesh_GetVertexArray(const Variant&)",
		asFUNCTION(NMesh_GetVertexArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Array<Variant>@ NMesh_GetFaceArray(const Variant&)",
		asFUNCTION(NMesh_GetFaceArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant NMesh_ConvertToTriMeshFromVariant(const Variant&)",
		asFUNCTION(NMesh_ConvertToTriMeshFromVariant),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant NMesh_ConvertToTriMeshFromVariantAndFaceTris(const Variant&, Array<Variant>@)",
		asFUNCTION(NMesh_ConvertToTriMeshFromVariantAndFaceTris),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Array<Variant>@ NMesh_ComputeWireframePolylinesArray(const Variant&)",
		asFUNCTION(NMesh_ComputeWireframePolylinesArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Variant NMesh_GetFacePolylineFromIndicesArray(const Variant&, int, Array<Variant>@)",
		asFUNCTION(NMesh_GetFacePolylineFromIndicesArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	if (failed) {
		URHO3D_LOGINFO("RegisterNMeshFunctions --- Failed to compile scripts");
	}
	else {
		URHO3D_LOGINFO("RegisterNMeshFunctions --- OK!");
	}

	return !failed;
}