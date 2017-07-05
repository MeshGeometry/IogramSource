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


#include "Curve_HelixSpiral.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ConversionUtilities.h"
#include "StockGeometries.h"
#include "Geomlib_TransformVertexList.h"
#include "Geomlib_TriMeshThicken.h"
#include "Polyline.h"

using namespace Urho3D;

String Curve_HelixSpiral::iconTexture = "";

Curve_HelixSpiral::Curve_HelixSpiral(Context* context) : IoComponentBase(context, 5, 1)
{
    SetName("Helix/Spiral");
    SetFullName("ConstructHelixPolyline");
    SetDescription("Construct a helix or spiral polyline");
    SetGroup(IoComponentGroup::CURVE);
    SetSubgroup("Primitive");
    
    inputSlots_[0]->SetName("lower_r");
    inputSlots_[0]->SetVariableName("r_L");
    inputSlots_[0]->SetDescription("Lower radius for helix");
    inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
    inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[0]->SetDefaultValue(Variant(1.0f));
    inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("upper_r");
	inputSlots_[1]->SetVariableName("r_U");
	inputSlots_[1]->SetDescription("Upper radius for helix");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(1.0f));
	inputSlots_[1]->DefaultSet();
    
    inputSlots_[2]->SetName("height");
    inputSlots_[2]->SetVariableName("h");
    inputSlots_[2]->SetDescription("Height of helix");
    inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
    inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[2]->SetDefaultValue(Variant(10.0f));
    inputSlots_[2]->DefaultSet();
    
    inputSlots_[3]->SetName("turns");
    inputSlots_[3]->SetVariableName("T");
    inputSlots_[3]->SetDescription("Number of turns");
    inputSlots_[3]->SetVariantType(VariantType::VAR_FLOAT);
    inputSlots_[3]->SetDefaultValue(Variant(10.0f));
    inputSlots_[3]->DefaultSet();
    
    inputSlots_[4]->SetName("resolution");
    inputSlots_[4]->SetVariableName("res");
    inputSlots_[4]->SetDescription("Polyline resolution");
    inputSlots_[4]->SetVariantType(VariantType::VAR_INT);
    inputSlots_[4]->SetDefaultValue(Variant(500));
    inputSlots_[4]->DefaultSet();
    
    outputSlots_[0]->SetName("Polyline");
    outputSlots_[0]->SetVariableName("P");
    outputSlots_[0]->SetDescription("Zig zag polyline");
    outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_HelixSpiral::SolveInstance(
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
        URHO3D_LOGWARNING("r_L must be a valid float or integer.");
        outSolveInstance[0] = Variant();
        return;
    }
    float r_L = inSolveInstance[0].GetFloat();


	// Verify input slot 1
	VariantType type1 = inSolveInstance[1].GetType();
	if (!(type1 == VariantType::VAR_FLOAT || type1 == VariantType::VAR_INT)) {
		URHO3D_LOGWARNING("r_U must be a valid float or integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	float r_U = inSolveInstance[1].GetFloat();
    
    // Verify input slot 2
    VariantType type2 = inSolveInstance[2].GetType();
    if (!(type2 == VariantType::VAR_FLOAT || type2 == VariantType::VAR_INT)) {
        URHO3D_LOGWARNING("h must be a valid float or integer.");
        outSolveInstance[0] = Variant();
        return;
    }
    float h = inSolveInstance[2].GetFloat();
    
    // Verify input slot 3
    VariantType type3 = inSolveInstance[3].GetType();
    if (!(type3 == VariantType::VAR_FLOAT || type3 == VariantType::VAR_INT)) {
        URHO3D_LOGWARNING("T must be a valid float or integer.");
        outSolveInstance[0] = Variant();
        return;
    }
    float turns = inSolveInstance[3].GetFloat();
    
    // Verify input slot 4
    VariantType type4 = inSolveInstance[4].GetType();
    if (type4 != VariantType::VAR_INT) {
        URHO3D_LOGWARNING("Res must be a valid integer.");
        outSolveInstance[0] = Variant();
        return;
    }
    int res = inSolveInstance[4].GetInt();
    
    ///////////////////
    // COMPONENT'S WORK
	Variant out;

	if (r_L == r_U)
		out = MakeHelix(r_L, h, turns, res);
	else
		out = MakeSpiral(r_L, r_U, h, turns, res);
    
    /////////////////
    // ASSIGN OUTPUTS
    
    outSolveInstance[0] = out;
}
