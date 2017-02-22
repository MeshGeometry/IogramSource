#pragma once

#include "IoComponentBase.h"
#include "IoDataTree.h"


class URHO3D_API Mesh_VertexTopology : public IoComponentBase {
    URHO3D_OBJECT(Mesh_VertexTopology, IoComponentBase)
public:
    Mesh_VertexTopology(Urho3D::Context* context);
    
//    void SolveInstance(
//                       const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
//                       Urho3D::Vector<Urho3D::Variant>& outSolveInstance
//                       );
    
    void AddInputSlot() = delete;
    void AddOutputSlot() = delete;
    void DeleteInputSlot(int index) = delete;
    void DeleteOutputSlot(int index) = delete;
    
    int LocalSolve();
    
    IoDataTree ComputeVertexStars(Urho3D::Variant meshWithData);
    
    static Urho3D::String iconTexture;
};
