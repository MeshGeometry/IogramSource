#include "Mesh_VertexTopology.h"


#include "TriMesh.h"
#include "MeshTopologyQueries.h"


using namespace Urho3D;

String Mesh_VertexTopology::iconTexture = "Textures/Icons/Mesh_VertexTopology.png";

Mesh_VertexTopology::Mesh_VertexTopology(Context* context) : IoComponentBase(context, 2, 2)
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
    
    inputSlots_[1]->SetName("VertexID");
    inputSlots_[1]->SetVariableName("ID");
    inputSlots_[1]->SetDescription("ID of vertex of interest");
    inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
    inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[1]->SetDefaultValue(0);
    inputSlots_[1]->DefaultSet();
    
    outputSlots_[0]->SetName("VertexStar");
    outputSlots_[0]->SetVariableName("V");
    outputSlots_[0]->SetDescription("Indices of vertices adjacent to V");
    outputSlots_[0]->SetVariantType(VariantType::VAR_INT);
    outputSlots_[0]->SetDataAccess(DataAccess::LIST);
    
    outputSlots_[1]->SetName("AdjFaces");
    outputSlots_[1]->SetVariableName("F");
    outputSlots_[1]->SetDescription("Indices of faces incident to V");
    outputSlots_[1]->SetVariantType(VariantType::VAR_INT);
    outputSlots_[1]->SetDataAccess(DataAccess::LIST);
    
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
    
    int vertID = inSolveInstance[1].GetInt();
    
    ///////////////////
    // COMPONENT'S WORK
    
    VariantVector vertex_star = TriMesh_VertexToVertices(inMesh, vertID);
    VariantVector adj_faces = TriMesh_VertexToFaces(inMesh, vertID);
    
    /////////////////
    // ASSIGN OUTPUTS
    
    outSolveInstance[0] = vertex_star;
    outSolveInstance[1] = adj_faces;
    
}
