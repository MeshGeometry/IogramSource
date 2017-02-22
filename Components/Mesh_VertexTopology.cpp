#include "Mesh_VertexTopology.h"


#include "TriMesh.h"
#include "MeshTopologyQueries.h"


using namespace Urho3D;

String Mesh_VertexTopology::iconTexture = "Textures/Icons/Mesh_VertexTopology.png";

Mesh_VertexTopology::Mesh_VertexTopology(Context* context) : IoComponentBase(context, 2, 3)
{
    SetName("VertexTopology");
    SetFullName("Compute Mesh Vertex Topology");
    SetDescription("Computes Mesh Topology Data for a given vertex");
    SetGroup(IoComponentGroup::MESH);
    SetSubgroup("Analysis");
    
    inputSlots_[0]->SetName("Mesh");
    inputSlots_[0]->SetVariableName("M");
    inputSlots_[0]->SetDescription("TriMeshWithAdjacencyData");
    inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
//    inputSlots_[1]->SetName("VertexID");
//    inputSlots_[1]->SetVariableName("ID");
//    inputSlots_[1]->SetDescription("ID of vertex of interest");
//    inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
//    inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
//    inputSlots_[1]->SetDefaultValue(0);
//    inputSlots_[1]->DefaultSet();
    
    outputSlots_[0]->SetName("VertexStarIDs");
    outputSlots_[0]->SetVariableName("V_ID");
    outputSlots_[0]->SetDescription("Indices of vertices adjacent to V");
    outputSlots_[0]->SetVariantType(VariantType::VAR_INT);
    outputSlots_[0]->SetDataAccess(DataAccess::LIST);
    
    outputSlots_[1]->SetName("VertexStar");
    outputSlots_[1]->SetVariableName("V");
    outputSlots_[1]->SetDescription("Vertices adjacent to V");
    outputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
    outputSlots_[1]->SetDataAccess(DataAccess::LIST);
    
    outputSlots_[2]->SetName("AdjFaces");
    outputSlots_[2]->SetVariableName("F");
    outputSlots_[2]->SetDescription("Indices of faces incident to V");
    outputSlots_[2]->SetVariantType(VariantType::VAR_INT);
    outputSlots_[2]->SetDataAccess(DataAccess::LIST);
    
}

void Mesh_VertexTopology::SolveInstance(
                                              const Vector<Variant>& inSolveInstance,
                                              Vector<Variant>& outSolveInstance
                                              )
{
    
    ///////////////////
    // VERIFY & EXTRACT
    
    // Verify input slot 0
    Variant inMesh = inSolveInstance[0];
    if (!TriMesh_HasAdjacencyData(inMesh)) {
        URHO3D_LOGWARNING("M must be a valid mesh with adjacency data.");
		SetAllOutputsNull(outSolveInstance);
        return;
    }
    

 //   int vertID = inSolveInstance[1].GetInt();
    
    VariantMap meshWithData = inMesh.GetVariantMap();
    VariantMap triMesh = meshWithData["mesh"].GetVariantMap();
    
    Urho3D::VariantVector vertexList = TriMesh_GetVertexList(Variant(triMesh));

//	// check that vertID is within range:
//	if (vertID < 0 || vertID > vertexList.Size()-1) {
//		URHO3D_LOGWARNING("VertexID is out of range");
//		SetAllOutputsNull(outSolveInstance);
//		return;
//	}
    
    ///////////////////
    // COMPONENT'S WORK
    
    VariantVector vertex_stars = meshWithData["vertex-vertex"].GetVariantVector();
    VariantVector adj_face_lists = meshWithData["vertex-face"].GetVariantVector();
    
//    VariantVector vertex_star = TriMesh_VertexToVertices(inMesh, vertID);
//    VariantVector adj_faces = TriMesh_VertexToFaces(inMesh, vertID);
//    
//    VariantVector vertexVectors;
//    for (int i = 0; i < vertex_star.Size(); ++i)
//    {
//        int v_ID = vertex_star[i].GetInt();
//        Vector3 curVert = vertexList[v_ID].GetVector3();
//        vertexVectors.Push(Variant(curVert));
//    }
    
    /////////////////
    // ASSIGN OUTPUTS
    
    outSolveInstance[0] = vertex_stars;
//    outSolveInstance[1] = vertexVectors;
    outSolveInstance[2] = adj_face_lists;
    
}
