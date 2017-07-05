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


#include "Mesh_Cylinder.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ConversionUtilities.h"
#include "StockGeometries.h"
#include "Geomlib_TransformVertexList.h"
#include "TriMesh.h"
#include "Geomlib_TriMeshThicken.h"

using namespace Urho3D;

String Mesh_Cylinder::iconTexture = "Textures/Icons/Mesh_Cylinder.png";

Mesh_Cylinder::Mesh_Cylinder(Context* context) : IoComponentBase(context, 4, 1)
{
    SetName("Cylinder");
    SetFullName("ConstructCylinderMesh");
    SetDescription("Construct a Cylinder mesh from radii");
    SetGroup(IoComponentGroup::MESH);
    SetSubgroup("Primitive");
    
    inputSlots_[0]->SetName("LowerRadius");
    inputSlots_[0]->SetVariableName("L");
    inputSlots_[0]->SetDescription("Lower radius");
    inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
    inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[0]->SetDefaultValue(Variant(1.0f));
    inputSlots_[0]->DefaultSet();
    
    inputSlots_[1]->SetName("UpperRadius");
    inputSlots_[1]->SetVariableName("U");
    inputSlots_[1]->SetDescription("Upper radius (0 for cone)");
    inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
    inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[1]->SetDefaultValue(Variant(1.0f));
    inputSlots_[1]->DefaultSet();
    
    inputSlots_[2]->SetName("Height");
    inputSlots_[2]->SetVariableName("h");
    inputSlots_[2]->SetDescription("Height");
    inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
    inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[2]->SetDefaultValue(Variant(4.0f));
    inputSlots_[2]->DefaultSet();
    
    inputSlots_[3]->SetName("Sides");
    inputSlots_[3]->SetVariableName("S");
    inputSlots_[3]->SetDescription("Integer (>3) describing number of sides");
    inputSlots_[3]->SetVariantType(VariantType::VAR_INT);
    inputSlots_[3]->SetDefaultValue(Variant(8));
    inputSlots_[3]->DefaultSet();
    
    //inputSlots_[4]->SetName("Transformation");
    //inputSlots_[4]->SetVariableName("T");
    //inputSlots_[4]->SetDescription("Transformation to apply to cube");
    //inputSlots_[4]->SetVariantType(VariantType::VAR_MATRIX3X4);
    //inputSlots_[4]->SetDefaultValue(Matrix3x4::IDENTITY);
    //inputSlots_[4]->DefaultSet();
    
    outputSlots_[0]->SetName("TriMesh");
    outputSlots_[0]->SetVariableName("M");
    outputSlots_[0]->SetDescription("Cylinder TriMesh");
    outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
    //outputSlots_[1]->SetName("QuadMesh");
    //outputSlots_[1]->SetVariableName("Q");
    //outputSlots_[1]->SetDescription("Cylinder QuadMesh");
    //outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
    //outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_Cylinder::SolveInstance(
                               const Vector<Variant>& inSolveInstance,
                               Vector<Variant>& outSolveInstance
                               )
{
    assert(inSolveInstance.Size() == inputSlots_.Size());
    assert(outSolveInstance.Size() == outputSlots_.Size());
    
    ///////////////////
    // VERIFY & EXTRACT
    
    // Verify input slot 0
    VariantType type0 = inSolveInstance[0].GetType();
    if (!(type0 == VariantType::VAR_FLOAT || type0 == VariantType::VAR_INT)) {
        URHO3D_LOGWARNING("Lower radius must be a valid float or integer.");
        outSolveInstance[0] = Variant();
        return;
    }
    float lower = inSolveInstance[0].GetFloat();
    if (lower <= 0.0f) {
        URHO3D_LOGWARNING("Lower radius must be > 0.");
        outSolveInstance[0] = Variant();
        return;
    }
    
    //verify input slot 1
    VariantType type1 = inSolveInstance[1].GetType();
    if (!(type1 == VariantType::VAR_FLOAT || type1 == VariantType::VAR_INT)) {
        URHO3D_LOGWARNING("Upper radius must be a valid float or integer.");
        outSolveInstance[0] = Variant();
        return;
    }
    float upper = inSolveInstance[1].GetFloat();
    if (upper < 0.0f) {
        URHO3D_LOGWARNING("Upper radius must be >= 0.");
        outSolveInstance[0] = Variant();
        return;
    }
    
    // Verify input slot 2
    VariantType type2 = inSolveInstance[2].GetType();
    if (!(type2 == VariantType::VAR_INT || type2 == VariantType::VAR_FLOAT)) {
        URHO3D_LOGWARNING("Height must be a valid integer or float.");
        outSolveInstance[0] = Variant();
        return;
    }
    float h = inSolveInstance[2].GetFloat();
    if (h <= 0.0f ) {
        URHO3D_LOGWARNING("Height must be positive");
        outSolveInstance[0] = Variant();
        return;
    }
    
    
    //// Verify input slot 4
    //VariantType type4 = inSolveInstance[4].GetType();
    //if (type4 != VariantType::VAR_MATRIX3X4) {
    //	URHO3D_LOGWARNING("T must be a valid transform.");
    //	outSolveInstance[0] = Variant();
    //	return;
    //}
    //Matrix3x4 tr = inSolveInstance[4].GetMatrix3x4();
    
    // Verify input slot 4
    int sides = inSolveInstance[3].GetInt();
    if (sides < 3) {
        //URHO3D_LOGWARNING("R must be larger than 3");
        //outSolveInstance[0] = Variant();
        //return;
        sides = 3;
    }
    
    ///////////////////
    // COMPONENT'S WORK
    Variant outMesh;
    if (h == 0)
        outMesh = MakeCone(sides, lower, h);
    else
        outMesh = MakeCylinder(sides, lower, upper, h);
    /////////////////
    // ASSIGN OUTPUTS
    
    outSolveInstance[0] = outMesh;
  //  outSolveInstance[1] = torusTriMesh;
}
