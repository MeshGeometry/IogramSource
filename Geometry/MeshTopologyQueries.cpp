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


#include "MeshTopologyQueries.h"

#include "TriMesh.h"

#include <Urho3D/IO/Log.h>
#include <Urho3D/AngelScript/Script.h>
#include <AngelScript/angelscript.h>

#pragma warning(push, 0)
#include <igl/vertex_triangle_adjacency.h>
#include <igl/triangle_triangle_adjacency.h>
#include <igl/adjacency_list.h>
#pragma warning(pop)

#define CHECK_GEO_REG(result) if (result <= 0) { \
		printf("geo_reg: FAIL\n"); \
		failed = true; \
	}

using Urho3D::Variant;
using Urho3D::VariantMap;
using Urho3D::Vector;
using Urho3D::VariantVector;
using Urho3D::VariantType;

Urho3D::Variant TriMesh_ComputeAdjacencyData(const Urho3D::Variant& triMesh)
{
    VariantMap triMeshWithData;
    
    Variant earlyRet;
    if (!TriMesh_Verify(triMesh))
        return earlyRet;
    
    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    TriMeshToMatrices(triMesh, V, F);
    
    // VERTEX-VERTEX
    std::vector<std::vector<int> > A;
    igl::adjacency_list(F, A);
    
    //Vector<Vector <int> > vertex_vertex_list;
    VariantVector vertex_vertex_list;
    for (int i = 0; i < A.size(); ++i){
        VariantVector vertex_star;
        for (int j = 0; j < A[i].size(); ++j){
            vertex_star.Push(Variant(A[i][j]));
        }
        vertex_vertex_list.Push(Variant(vertex_star));
    }
    
    // VERTEX-FACE
    std::vector<std::vector<int> > VF;
    std::vector<std::vector<int> > VFi;
    igl::vertex_triangle_adjacency(V, F, VF, VFi);
    
    //Vector<Vector <int> > vertex_face_list;
    VariantVector vertex_face_list;
    for (int i = 0; i < VF.size(); ++i){
        VariantVector adj_faces;
        for (int j = 0; j < VF[i].size(); ++j){
            adj_faces.Push(Variant(VF[i][j]));
        }
        vertex_face_list.Push(Variant(adj_faces));
    }
    
    // FACE-FACE
    Eigen::MatrixXi TT;
    Eigen::MatrixXi TTi;
    igl::triangle_triangle_adjacency(F, TT, TTi);

    VariantVector face_face_list;
    for (int i = 0; i < TT.rows(); ++i){
        VariantVector adj_faces;
        for (int j = 0; j < 3; ++j){
            adj_faces.Push(Variant(TT(i,j)));
        }
        face_face_list.Push(Variant(adj_faces));
    }
    
    triMeshWithData["type"] = Variant(Urho3D::String("TriMeshWithData"));
    triMeshWithData["mesh"] = triMesh;
    triMeshWithData["vertex-vertex"] = Variant(vertex_vertex_list);
    triMeshWithData["vertex-face"] = Variant(vertex_face_list);
    triMeshWithData["face-face"] = Variant(face_face_list);
    
    return Variant(triMeshWithData);

}

bool TriMesh_HasAdjacencyData(const Urho3D::Variant& triMesh)
{
    if (triMesh.GetType() != VariantType::VAR_VARIANTMAP) return false;
    
    VariantMap var_map = triMesh.GetVariantMap();
    Variant var_type = var_map["type"];
    if (var_type.GetType() != VariantType::VAR_STRING) return false;
    
    if (var_type.GetString() != "TriMeshWithData") return false;
    
    return true;
}


// VERTEX QUERIES
Urho3D::VariantVector TriMesh_VertexToVertices(Urho3D::Variant& triMeshWithData, int vertID)
{
 //   if (!TriMesh_HasAdjacencyData(triMeshWithData))
 //       return VariantVector();
 //   
 //   VariantMap* triMeshWith = triMeshWithData.GetVariantMapPtr();
 //   VariantVector* vertex_vertex_list = (*triMeshWith)["vertex-vertex"].GetVariantVectorPtr();
	//int num = vertex_vertex_list->Size();
 //   if (vertID < num && vertID > -1)
 //       return (*vertex_vertex_list)[vertID].GetVariantVector();
 //   else{
 //       URHO3D_LOGWARNING("vertex ID out of range");
 //       return VariantVector();
 //   }
	

	return VariantVector();
}

Urho3D::Vector<Urho3D::Variant> TriMesh_VertexToVertices(Urho3D::Variant& triMeshWithData)
{
    if (!TriMesh_HasAdjacencyData(triMeshWithData))
        return Vector<Urho3D::Variant> ();
    
    VariantMap* triMeshWith = triMeshWithData.GetVariantMapPtr();
    VariantVector* vertex_vertex_list = (*triMeshWith)["vertex-vertex"].GetVariantVectorPtr();
    Vector<Urho3D::Variant> vertex_stars;
    int num = vertex_vertex_list->Size();
    for (int i = 0; i < num-1; ++i)
    {
        vertex_stars.Push(vertex_vertex_list->At(i));
    }
    return vertex_stars;

}


Urho3D::VariantVector TriMesh_VertexToFaces(Urho3D::Variant& triMeshWithData, int vertID)
{
    
    if (!TriMesh_HasAdjacencyData(triMeshWithData))
        return VariantVector();
    
	VariantMap* triMeshWith = triMeshWithData.GetVariantMapPtr();
    VariantVector* vertex_face_list = (*triMeshWith)["vertex-face"].GetVariantVectorPtr();
	int num = vertex_face_list->Size();
    if (vertID < num && vertID > -1)
        return (*vertex_face_list)[vertID].GetVariantVector();
    else{
        URHO3D_LOGWARNING("vertex ID out of range");
        return VariantVector();
    }

}

Urho3D::VariantVector TriMesh_FaceToVertices(const Urho3D::Variant& triMeshWithData, int faceID)
{
    if (!TriMesh_HasAdjacencyData(triMeshWithData))
        return VariantVector();
    
    VariantMap triMeshWith = triMeshWithData.GetVariantMap();
    VariantMap triMesh = triMeshWith["mesh"].GetVariantMap();
    VariantVector face_list = TriMesh_GetFaceList(triMesh);
    
    if (faceID > face_list.Size()/3){
        URHO3D_LOGWARNING("face ID out of range");
        return VariantVector();
    }
    
    VariantVector face_vertices;
    face_vertices.Push(face_list[3*faceID].GetInt());
    face_vertices.Push(face_list[3*faceID + 1].GetInt());
    face_vertices.Push(face_list[3*faceID + 2].GetInt());
    
    return face_vertices;
}

Urho3D::VariantVector TriMesh_FaceToFaces(const Urho3D::Variant& triMeshWithData, int faceID)
{
    if (!TriMesh_HasAdjacencyData(triMeshWithData))
        return VariantVector();
    
    VariantMap triMeshWith = triMeshWithData.GetVariantMap();
    VariantVector face_face_list = triMeshWith["face-face"].GetVariantVector();
    
    if (faceID < face_face_list.Size() && faceID > -1)
        return face_face_list[faceID].GetVariantVector();
    else{
        URHO3D_LOGWARNING("face ID out of range");
        return VariantVector();
    }
}

// for scripts

Urho3D::CScriptArray* TriMesh_VertexToVerticesArrayFromId(Urho3D::Variant& triMeshWithData, int vertID)
{
	Vector<Variant> vertices_data = TriMesh_VertexToVertices(triMeshWithData, vertID);
	return Urho3D::VectorToArray<Variant>(vertices_data, "Array<Variant>");
}

Urho3D::CScriptArray* TriMesh_VertexToVerticesArray(Urho3D::Variant& triMeshWithData)
{
	Vector<Variant> vertices_data = TriMesh_VertexToVertices(triMeshWithData);
	return Urho3D::VectorToArray<Variant>(vertices_data, "Array<Variant>");
}

Urho3D::CScriptArray* TriMesh_VertexToFacesArray(Urho3D::Variant& triMeshWithData, int vertID)
{
	Vector<Variant> faces_data = TriMesh_VertexToFaces(triMeshWithData, vertID);
	return Urho3D::VectorToArray<Variant>(faces_data, "Array<Variant>");
}

Urho3D::CScriptArray* TriMesh_FaceToVerticesArray(const Urho3D::Variant& triMeshWithData, int faceID)
{
	Vector<Variant> vertices_data = TriMesh_FaceToVertices(triMeshWithData, faceID);
	return Urho3D::VectorToArray<Variant>(vertices_data, "Array<Variant>");
}

Urho3D::CScriptArray* TriMesh_FaceToFacesArray(const Urho3D::Variant& triMeshWithData, int faceID)
{
	Vector<Variant> faces_data = TriMesh_FaceToFaces(triMeshWithData, faceID);
	return Urho3D::VectorToArray<Variant>(faces_data, "Array<Variant>");
}

bool RegisterMeshTopologyQueryFunctions(Urho3D::Context* context)
{
	Urho3D::Script* script_system = context->GetSubsystem<Urho3D::Script>();
	asIScriptEngine* engine = script_system->GetScriptEngine();

	bool failed = false;

	int res;
	res = engine->RegisterGlobalFunction(
		"Variant TriMesh_ComputeAdjacencyData(const Variant&)",
		asFUNCTION(TriMesh_ComputeAdjacencyData),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"bool TriMesh_HasAdjacencyData(const Variant&)",
		asFUNCTION(TriMesh_HasAdjacencyData),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Array<Variant>@ TriMesh_VertexToVerticesArrayFromId(Variant&, int)",
		asFUNCTION(TriMesh_VertexToVerticesArrayFromId),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Array<Variant>@ TriMesh_VertexToVerticesArray(Variant&)",
		asFUNCTION(TriMesh_VertexToVerticesArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Array<Variant>@ TriMesh_VertexToFacesArray(Variant&, int)",
		asFUNCTION(TriMesh_VertexToFacesArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Array<Variant>@ TriMesh_FaceToVerticesArray(const Variant&, int)",
		asFUNCTION(TriMesh_FaceToVerticesArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Array<Variant>@ TriMesh_FaceToFacesArray(const Variant&, int)",
		asFUNCTION(TriMesh_FaceToFacesArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	if (failed) {
		URHO3D_LOGINFO("RegisterTopologyQueryFunctions --- Failed to compile scripts");
	}
	else {
		URHO3D_LOGINFO("RegisterTopologyQueryFunctions --- OK!");
	}

	return !failed;
}