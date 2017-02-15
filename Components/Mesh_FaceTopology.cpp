#include "Mesh_FaceTopology.h"


#include "TriMesh.h"
#include "MeshTopologyQueries.h"


using namespace Urho3D;

String Mesh_FaceTopology::iconTexture = "Textures/Icons/Mesh_FaceTopology.png";

Mesh_FaceTopology::Mesh_FaceTopology(Context* context) : IoComponentBase(context, 2, 3)
{
    SetName("FaceTopology");
    SetFullName("Compute Mesh Face Topology");
    SetDescription("Computes Mesh Topology Data for a given face");
    SetGroup(IoComponentGroup::MESH);
    SetSubgroup("Analysis");
    
    inputSlots_[0]->SetName("Mesh");
    inputSlots_[0]->SetVariableName("M");
    inputSlots_[0]->SetDescription("TriMeshWithAdjacencyData");
    inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
    inputSlots_[1]->SetName("FaceID");
    inputSlots_[1]->SetVariableName("ID");
    inputSlots_[1]->SetDescription("ID of face of interest");
    inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
    inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[1]->SetDefaultValue(0);
    inputSlots_[1]->DefaultSet();
    
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

void Mesh_FaceTopology::SolveInstance(
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
        outSolveInstance[0] = Variant();
        return;
    }
    
    // Verify input slot 1
    VariantType type = inSolveInstance[1].GetType();
    if (!(type == VariantType::VAR_INT)) {
        URHO3D_LOGWARNING("ID must be a valid integer.");
        outSolveInstance[0] = Variant();
        return;
    }
    
    int faceID = inSolveInstance[1].GetInt();
    VariantMap meshWithData = inMesh.GetVariantMap();
    VariantMap triMesh = meshWithData["mesh"].GetVariantMap();
    
    Urho3D::VariantVector vertexList = TriMesh_GetVertexList(Variant(triMesh));
    
    
    
    ///////////////////
    // COMPONENT'S WORK
    
    VariantVector vert_ids = TriMesh_FaceToVertices(inMesh, faceID);
    VariantVector adj_faces = TriMesh_FaceToFaces(inMesh, faceID);
    VariantVector vertexVectors;
    for (int i = 0; i < vert_ids.Size(); ++i)
    {
        int v_ID = vert_ids[i].GetInt();
        Vector3 curVert = vertexList[v_ID].GetVector3();
        vertexVectors.Push(Variant(curVert));
    }
    
    
    /////////////////
    // ASSIGN OUTPUTS
    
    outSolveInstance[0] = vert_ids;
    outSolveInstance[1] = vertexVectors;
    outSolveInstance[2] = adj_faces;
    
}
