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


#include "Mesh_FaceTopology.h"


#include "TriMesh.h"
#include "MeshTopologyQueries.h"


using namespace Urho3D;

String Mesh_FaceTopology::iconTexture = "Textures/Icons/Mesh_FaceTopology.png";

Mesh_FaceTopology::Mesh_FaceTopology(Context* context) : IoComponentBase(context, 1, 3)
{
    SetName("FaceTopology");
    SetFullName("Compute Mesh Face Topology");
    SetDescription("Computes Mesh Topology Data for face");
    SetGroup(IoComponentGroup::MESH);
    SetSubgroup("Analysis");
    
    inputSlots_[0]->SetName("Mesh");
    inputSlots_[0]->SetVariableName("M");
    inputSlots_[0]->SetDescription("TriMeshWithAdjacencyData");
    inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
       
    outputSlots_[0]->SetName("IncidentVertIDs");
    outputSlots_[0]->SetVariableName("V_ID");
    outputSlots_[0]->SetDescription("Indices of vertices incident to F");
    outputSlots_[0]->SetVariantType(VariantType::VAR_INT);
    outputSlots_[0]->SetDataAccess(DataAccess::LIST);
    
    outputSlots_[1]->SetName("IncidentVertices");
    outputSlots_[1]->SetVariableName("V");
    outputSlots_[1]->SetDescription("Vertices incident to F");
    outputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
    outputSlots_[1]->SetDataAccess(DataAccess::LIST);
    
    outputSlots_[2]->SetName("AdjFaces");
    outputSlots_[2]->SetVariableName("F");
    outputSlots_[2]->SetDescription("Indices of faces adjacent to F");
    outputSlots_[2]->SetVariantType(VariantType::VAR_INT);
    outputSlots_[2]->SetDataAccess(DataAccess::LIST);
    
}


int Mesh_FaceTopology::LocalSolve()
{
	assert(inputSlots_.Size() >= 1);
	assert(outputSlots_.Size() == 3);

	Urho3D::Vector<int> path;
	path.Push(0);

	IoDataTree nullTree(GetContext());
	nullTree.Add(path, Variant());

	if (inputSlots_[0]->HasNoData()) {
		solvedFlag_ = 0;
		outputSlots_[0]->SetIoDataTree(nullTree);
		outputSlots_[1]->SetIoDataTree(nullTree);
		outputSlots_[2]->SetIoDataTree(nullTree);
		return 0;
	}

	IoDataTree inputMeshTree = inputSlots_[0]->GetIoDataTree();

	Variant inMesh;


	// for some reason the component was crashing on initialization without this:
	inputMeshTree.GetItem(inMesh, path, 0);
	if (inMesh.GetType() == VAR_NONE)
	{
		solvedFlag_ = 0;
		outputSlots_[0]->SetIoDataTree(nullTree);
		outputSlots_[1]->SetIoDataTree(nullTree);
		outputSlots_[2]->SetIoDataTree(nullTree);
		return 0;
	}


	if (!TriMesh_HasAdjacencyData(inMesh)) {
		URHO3D_LOGWARNING("M must be a TriMesh WITH DATA (use Mesh_ComputeAdjacencyData)!");
		solvedFlag_ = 0;
		outputSlots_[0]->SetIoDataTree(nullTree);
		outputSlots_[1]->SetIoDataTree(nullTree);
		outputSlots_[2]->SetIoDataTree(nullTree);
		return 0;
	}

	IoDataTree face_vertex_vectors(GetContext());
	IoDataTree face_vertex_IDs = ComputeVerticesOnFace(inMesh, face_vertex_vectors);
	IoDataTree adjacent_faces = ComputeAdjacentFaces(inMesh);

	outputSlots_[0]->SetIoDataTree(face_vertex_IDs);
	outputSlots_[1]->SetIoDataTree(face_vertex_vectors);
	outputSlots_[2]->SetIoDataTree(adjacent_faces);

	solvedFlag_ = 1;
	return 1;
}

IoDataTree Mesh_FaceTopology::ComputeVerticesOnFace(Urho3D::Variant inMeshWithData, IoDataTree& vertexVectorsTree)
{
	IoDataTree face_vertex_tree(GetContext());

	VariantMap* meshWithData = inMeshWithData.GetVariantMapPtr();
	VariantMap* triMesh = (*meshWithData)["mesh"].GetVariantMapPtr();

	Urho3D::VariantVector vertexList = TriMesh_GetVertexList(Variant(*triMesh));
	VariantVector face_list = TriMesh_GetFaceList(Variant(*triMesh));

	int num = face_list.Size() / 3;

	for (int i = 0; i < num; ++i) {
		Vector<int> path;
		path.Push(i);

		VariantVector vertex_IDs; 
		vertex_IDs.Push(face_list[3 * i].GetInt());
		vertex_IDs.Push(face_list[3 * i + 1].GetInt());
		vertex_IDs.Push(face_list[3 * i + 2].GetInt());

		face_vertex_tree.Add(path, vertex_IDs);

		// compute the vectors of these verts at the same time.
		VariantVector vertex_vectors;
		for (int j = 0; j < vertex_IDs.Size(); ++j) {
			int curVert = vertex_IDs[j].GetInt();
			vertex_vectors.Push(Variant(vertexList[curVert].GetVector3()));
		}
		vertexVectorsTree.Add(path, vertex_vectors);
	}

	return face_vertex_tree;
}

IoDataTree Mesh_FaceTopology::ComputeAdjacentFaces(Urho3D::Variant inMeshWithData)
{
	IoDataTree adjacent_faces_tree(GetContext());

	VariantMap* meshWithData = inMeshWithData.GetVariantMapPtr();
	VariantMap* triMesh = (*meshWithData)["mesh"].GetVariantMapPtr();

	VariantVector* face_face_list = (*meshWithData)["face-face"].GetVariantVectorPtr();

	for (int i = 0; i < (*face_face_list).Size(); ++i) {
		Vector<int> path;
		path.Push(i);

		VariantVector adj_faces = (*face_face_list)[i].GetVariantVector();
		adjacent_faces_tree.Add(path, adj_faces);

	}

	return adjacent_faces_tree;
}