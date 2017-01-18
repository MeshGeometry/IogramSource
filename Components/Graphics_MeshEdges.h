#pragma once

#include "IoComponentBase.h"

class URHO3D_API Graphics_MeshEdges : public IoComponentBase {
    
    URHO3D_OBJECT(Graphics_MeshEdges, IoComponentBase)
    
public:
    Graphics_MeshEdges(Urho3D::Context* context);
    
    static Urho3D::String iconTexture;
    
    virtual void PreLocalSolve();
    
    void SolveInstance(
                       const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
                       Urho3D::Vector<Urho3D::Variant>& outSolveInstance
                       );
    
    Urho3D::Vector<int> trackedItems;
    
};
