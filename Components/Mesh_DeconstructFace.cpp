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


#include "Mesh_DeconstructFace.h"

#include "TriMesh.h"
#include "MeshTopologyQueries.h"
#include "ConversionUtilities.h"


using namespace Urho3D;

String Mesh_DeconstructFace::iconTexture = "Textures/Icons/Mesh_DeconstructFace.png";

Mesh_DeconstructFace::Mesh_DeconstructFace(Context* context) : IoComponentBase(context, 1, 4)
{
	SetName("DeconstructFace");
	SetFullName("Compute Mesh Face Data");
	SetDescription("Returns normal, centroid and vertices of face");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Analysis");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("TriMesh");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

//	inputSlots_[1]->SetName("FaceID");
//	inputSlots_[1]->SetVariableName("ID");
//	inputSlots_[1]->SetDescription("ID of face of interest");
//	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
//	inputSlots_[1]->SetDataAccess(DataAccess::LIST);
//	inputSlots_[1]->SetDefaultValue(0);
//	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("FaceVertexIDs");
	outputSlots_[0]->SetVariableName("V_ID");
	outputSlots_[0]->SetDescription("Indices of vertices on mesh face");
	outputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[1]->SetName("FaceVertices");
	outputSlots_[1]->SetVariableName("V");
	outputSlots_[1]->SetDescription("Vertices on face");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[1]->SetDataAccess(DataAccess::LIST);

	outputSlots_[2]->SetName("FaceNormal");
	outputSlots_[2]->SetVariableName("N");
	outputSlots_[2]->SetDescription("Normal to face");
	outputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[2]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[3]->SetName("FaceCentroid");
	outputSlots_[3]->SetVariableName("C");
	outputSlots_[3]->SetDescription("Centroid of face");
	outputSlots_[3]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[3]->SetDataAccess(DataAccess::ITEM);

}

// UPDATE: re-do to match vertex topology. That is, return data for ALL face IDs.

int Mesh_DeconstructFace::LocalSolve()
{
    assert(inputSlots_.Size() >= 1);
    assert(outputSlots_.Size() == 4);
    
    Urho3D::Vector<int> path;
    path.Push(0);
    
    IoDataTree nullTree(GetContext());
    nullTree.Add(path, Variant());
    
    if (inputSlots_[0]->HasNoData()) {
        solvedFlag_ = 0;
        outputSlots_[0]->SetIoDataTree(nullTree);
        outputSlots_[1]->SetIoDataTree(nullTree);
        outputSlots_[2]->SetIoDataTree(nullTree);
        outputSlots_[3]->SetIoDataTree(nullTree);
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
        outputSlots_[3]->SetIoDataTree(nullTree);
        return 0;
    }
    
    if (!TriMesh_Verify(inMesh)) {
        URHO3D_LOGWARNING("M must be a TriMesh!");
        solvedFlag_ = 0;
        outputSlots_[0]->SetIoDataTree(nullTree);
        outputSlots_[1]->SetIoDataTree(nullTree);
        outputSlots_[2]->SetIoDataTree(nullTree);
        outputSlots_[3]->SetIoDataTree(nullTree);
        return 0;
    }
    
    // Compute the data
    IoDataTree faceVertexVectors_tree(GetContext());
    IoDataTree centroids_tree(GetContext());

    IoDataTree faceVertexIDs_tree = GetVerticesOnFaces(inMesh, faceVertexVectors_tree, centroids_tree);
    IoDataTree faceNormals_tree = GetFaceNormals(inMesh);
    
    outputSlots_[0]->SetIoDataTree(faceVertexIDs_tree);
    outputSlots_[1]->SetIoDataTree(faceVertexVectors_tree);
    outputSlots_[2]->SetIoDataTree(faceNormals_tree);
    outputSlots_[3]->SetIoDataTree(centroids_tree);
    
    solvedFlag_ = 1;
    return 1;

}

IoDataTree Mesh_DeconstructFace::GetVerticesOnFaces(Urho3D::Variant inMesh, IoDataTree& vertexVectorsTree,
    IoDataTree& centroidTree)
{
    IoDataTree vertex_IDs_tree(GetContext());
    
    VariantMap* triMesh = inMesh.GetVariantMapPtr();
    Urho3D::VariantVector faceList = TriMesh_GetFaceList(Variant(*triMesh));
    Urho3D::VariantVector vertexList = TriMesh_GetVertexList(Variant(*triMesh));
    
    for (int i = 0; i < faceList.Size(); i+=3){
        Vector<int> path;
        path.Push(i);
        
        VariantVector curFaceIDs;
        VariantVector curFaceVertVectors;
        
        int tri_a = faceList[i].GetInt();
        int tri_b = faceList[i+1].GetInt();
        int tri_c = faceList[i+2].GetInt();
        
        curFaceIDs.Push(tri_a);
        curFaceIDs.Push(tri_b);
        curFaceIDs.Push(tri_c);
        
        Vector3 vec_a = vertexList[tri_a].GetVector3();
        Vector3 vec_b = vertexList[tri_b].GetVector3();
        Vector3 vec_c = vertexList[tri_c].GetVector3();
        
        curFaceVertVectors.Push(Variant(vec_a));
        curFaceVertVectors.Push(Variant(vec_b));
        curFaceVertVectors.Push(Variant(vec_c));
        
        Vector3 centroid = (vec_a + vec_b + vec_c)/3;
        
        vertex_IDs_tree.Add(path, curFaceIDs);
        vertexVectorsTree.Add(path, curFaceVertVectors);
        centroidTree.Add(path, Variant(centroid));
    }
    
    return vertex_IDs_tree;
}


IoDataTree Mesh_DeconstructFace::GetFaceNormals(Urho3D::Variant inMesh)
{
    VariantVector normalList = TriMesh_ComputeFaceNormals(inMesh, true);
    
    IoDataTree faceNormals_tree(GetContext());
    
    for (int i = 0; i < normalList.Size(); ++i){
        Vector<int> path;
        path.Push(i);
        
        Vector3 normal = normalList[i].GetVector3();
        faceNormals_tree.Add(path, Variant(normal));
        
    }
    
    return faceNormals_tree;
}



//
//void Mesh_DeconstructFace::SolveInstance(
//	const Vector<Variant>& inSolveInstance,
//	Vector<Variant>& outSolveInstance
//	)
//{
//
//	///////////////////
//	// VERIFY & EXTRACT
//
//	// Verify input slot 0
//	Variant inMesh = inSolveInstance[0];
//	if (!TriMesh_Verify(inMesh)) {
//		URHO3D_LOGWARNING("M must be a valid mesh.");
//		SetAllOutputsNull(outSolveInstance);
//		return;
//	}
//
//	// Verify input slot 1
//    VariantVector faceIdx = inSolveInstance[1].GetVariantVector();
//	VariantType type1 = faceIdx[0].GetType();
//	if (!(type1 == VariantType::VAR_INT)) {
//		URHO3D_LOGWARNING("FaceID must be a valid integer.");
//		SetAllOutputsNull(outSolveInstance);
//		return;
//	}
//
//	
//
//	VariantVector vertexList = TriMesh_GetVertexList(inMesh);
//	VariantVector faceList = TriMesh_GetFaceList(inMesh);
//	VariantVector normalList = TriMesh_ComputeFaceNormals(inMesh, true);
//
//	// check that faceIDs are within range:
//    for (int i = 0; i < faceIdx.Size(); ++i){
//        int faceID = faceIdx[i].GetInt();
//        if (faceID < 0 || faceID > normalList.Size()-1) {
//            URHO3D_LOGWARNING("FaceID is out of range");
//            SetAllOutputsNull(outSolveInstance);
//            return;
//        }
//    }
//
//	///////////////////
//	// COMPONENT'S WORK
//    
//    // set up output lists:
//    VariantVector faceVertexIDs_list;
//    VariantVector faceVertexVectors_list;
//    VariantVector centroids_list;
//    VariantVector faceNormals_list;
//    
//    // iterate over all faceIDs
//    for (int i = 0; i < faceIdx.Size(); ++i){
//        int faceID = faceIdx[i].GetInt();
//
//        Vector3 normal = normalList[faceID].GetVector3();
//        normal = normal.Normalized();
//
//        VariantVector faceVertexIDs;
//        VariantVector faceVertexVectors;
//        Vector3 centroid = Vector3::ZERO;
//
//
//        // get the vert IDs
//        for (int i = 0; i < 3; ++i)
//        {
//            faceVertexIDs.Push(faceList[3 * faceID + i].GetInt());
//        }
//
//        // get the vertex vectors
//        for (int i = 0; i < faceVertexIDs.Size(); ++i) {
//            int vertID = faceVertexIDs[i].GetInt();
//            Vector3 curVert = vertexList[vertID].GetVector3();
//            faceVertexVectors.Push(curVert);
//            centroid += curVert;
//        }
//
//        // find the actual centroid
//        centroid = centroid / 3;
//        
//        // add data to relevant lists
//        faceVertexIDs_list.Push(Variant(faceVertexIDs));
//        faceVertexVectors_list.Push(Variant(faceVertexVectors));
//        centroids_list.Push(Variant(centroid));
//        faceNormals_list.Push(Variant(normal));
//        
//    }
//
//
////	outSolveInstance[0] = Variant(faceVertexIDs);
////	outSolveInstance[1] = Variant(faceVertexVectors);
////	outSolveInstance[2] = Variant(normal);
////	outSolveInstance[3] = Variant(centroid);
//
//    outSolveInstance[0] = Variant(faceVertexIDs_list);
//    outSolveInstance[1] = Variant(faceVertexVectors_list);
//    outSolveInstance[2] = Variant(faceNormals_list);
//    outSolveInstance[3] = Variant(centroids_list);
//
//}
