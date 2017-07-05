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


#include "Curve_PolylineSweep.h"

#include <assert.h>

#include "Polyline.h"
#include "Geomlib_PolylineSweep.h"

using namespace Urho3D;

Urho3D::String Curve_PolylineSweep::iconTexture = "Textures/Icons/Curve_PolylineSweep.png";

Curve_PolylineSweep::Curve_PolylineSweep(Context* context) : IoComponentBase(context, 3, 1)
{
    SetName("PolylineSweep");
    SetFullName("Polyline Sweep");
    SetDescription("Sweeps a section curve along one or two rail curves");
    SetGroup(IoComponentGroup::CURVE);
    SetSubgroup("Operators");
    
    inputSlots_[0]->SetName("Section");
    inputSlots_[0]->SetVariableName("S");
    inputSlots_[0]->SetDescription("Section polyline");
    inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
    inputSlots_[1]->SetName("Rail_1");
    inputSlots_[1]->SetVariableName("R1");
    inputSlots_[1]->SetDescription("Rail polyline");
    inputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
    inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
    
    inputSlots_[2]->SetName("Rail_2");
    inputSlots_[2]->SetVariableName("R2");
    inputSlots_[2]->SetDescription("Optional Second Rail polyline");
    inputSlots_[2]->SetVariantType(VariantType::VAR_VARIANTMAP);
    inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
    
    outputSlots_[0]->SetName("Mesh");
    outputSlots_[0]->SetVariableName("M");
    outputSlots_[0]->SetDescription("Sweep mesh");
    outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_PolylineSweep::SolveInstance(
                                        const Vector<Variant>& inSolveInstance,
                                        Vector<Variant>& outSolveInstance
                                        )
{
    ///////////////////
    // VERIFY & EXTRACT
    
    // Verify input slot 0
    Variant polyIn1 = inSolveInstance[0];
    if (!Polyline_Verify(polyIn1)) {
        URHO3D_LOGWARNING("P1 must be a valid polyline.");
        outSolveInstance[0] = Variant();
        return;
    }
    // Verify input slot 1
    Variant polyIn2 = inSolveInstance[1];
    if (!Polyline_Verify(polyIn2)) {
        URHO3D_LOGWARNING("P2 must be a valid polyline.");
        outSolveInstance[0] = Variant();
        return;
    }
    
    // Verify input slot 2
    Variant polyIn3 = inSolveInstance[2];
    bool two_rails = Polyline_Verify(polyIn3);
    if (!two_rails) {
        URHO3D_LOGWARNING("No second rail, performing single-rail sweep.");
    }
    
    ///////////////////
    // COMPONENT'S WORK
    
    Variant mesh;
    bool success = false;
    if (!two_rails)
        success = Geomlib::PolylineSweep_single(polyIn2, polyIn1, mesh);
    else
        success = Geomlib::PolylineSweep_double(polyIn2, polyIn3, polyIn1, mesh);
    if (!success) {
        URHO3D_LOGWARNING("PolylineBlend operation failed.");
        outSolveInstance[0] = Variant();
        return;
    }
    
    /////////////////
    // ASSIGN OUTPUTS
    
    outSolveInstance[0] = mesh;
}
