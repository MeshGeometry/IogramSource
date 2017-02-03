#include "Mesh_ComputeAdjacencyData.h"


#include "TriMesh.h"
#include "MeshTopologyQueries.h"


using namespace Urho3D;

String Mesh_ComputeAdjacencyData::iconTexture = "Textures/Icons/Mesh_ComputeAdjacencyData.png";

Mesh_ComputeAdjacencyData::Mesh_ComputeAdjacencyData(Context* context) : IoComponentBase(context, 1, 1)
{
    SetName("ComputeAdjacencyData");
    SetFullName("Compute Mesh Topology Data");
    SetDescription("Computes Mesh Topology Data (manifold trimeshes only!)");
    SetGroup(IoComponentGroup::MESH);
    SetSubgroup("Operators");
    
    inputSlots_[0]->SetName("Mesh");
    inputSlots_[0]->SetVariableName("M");
    inputSlots_[0]->SetDescription("TriMesh");
    inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
    outputSlots_[0]->SetName("Mesh");
    outputSlots_[0]->SetVariableName("M");
    outputSlots_[0]->SetDescription("Mesh with topology data computed");
    outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
}

void Mesh_ComputeAdjacencyData::SolveInstance(
                                   const Vector<Variant>& inSolveInstance,
                                   Vector<Variant>& outSolveInstance
                                   )
{
    
    ///////////////////
    // VERIFY & EXTRACT
    
    // Verify input slot 0
    Variant inMesh = inSolveInstance[0];
    if (!TriMesh_Verify(inMesh)) {
        URHO3D_LOGWARNING("M must be a valid mesh.");
        outSolveInstance[0] = Variant();
        return;
    }
    
    ///////////////////
    // COMPONENT'S WORK
    
    Variant out_mesh = TriMesh_ComputeAdjacencyData(inMesh);
    
    /////////////////
    // ASSIGN OUTPUTS
    
    outSolveInstance[0] = out_mesh;

}
