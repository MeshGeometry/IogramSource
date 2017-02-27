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


#include "Mesh_FlipNormals.h"

#include <Urho3D/Core/Variant.h>

#include "TriMesh.h"

using namespace Urho3D;

String Mesh_FlipNormals::iconTexture = "";

Mesh_FlipNormals::Mesh_FlipNormals(Context* context) : IoComponentBase(context, 1, 1)
{
    SetName("FlipNormals");
    SetFullName("Unify Normals");
    SetDescription("Unify TriMesh normals to consistent orientation");
    SetGroup(IoComponentGroup::MESH);
    SetSubgroup("Operators");
    
    inputSlots_[0]->SetName("MeshIn");
    inputSlots_[0]->SetVariableName("M");
    inputSlots_[0]->SetDescription("Mesh in");
    inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
    outputSlots_[0]->SetName("MeshOut");
    outputSlots_[0]->SetVariableName("N");
    outputSlots_[0]->SetDescription("Mesh with flipped normals");
    outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_FlipNormals::SolveInstance(
                                      const Vector<Variant>& inSolveInstance,
                                      Vector<Variant>& outSolveInstance
                                      )
{
    Variant mesh_in = inSolveInstance[0];
    if (!TriMesh_Verify(mesh_in)) {
        URHO3D_LOGWARNING("M must be a TriMesh!");
        SetAllOutputsNull(outSolveInstance);
        return;
    }
    
    Variant mesh_out = TriMesh_FlipNormals(mesh_in);
    
    outSolveInstance[0] = mesh_out;
}
