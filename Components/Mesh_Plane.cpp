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

#include "Mesh_Plane.h"

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

String Mesh_Plane::iconTexture = "Textures/Icons/Mesh_Plane.png";

Mesh_Plane::Mesh_Plane(Context* context) : IoComponentBase(context, 1, 3)
{
	SetName("Plane");
	SetFullName("ConstructPlaneMesh");
	SetDescription("Construct a plane mesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("Height");
	inputSlots_[0]->SetVariableName("Height");
	inputSlots_[0]->SetDescription("Offset from origin");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant(0.0f));
	inputSlots_[0]->DefaultSet();

//	inputSlots_[1]->SetName("Transformation");
//	inputSlots_[1]->SetVariableName("T");
//	inputSlots_[1]->SetDescription("Transformation to apply to plane");
//	inputSlots_[1]->SetVariantType(VariantType::VAR_MATRIX3X4);
//	inputSlots_[1]->SetDefaultValue(Matrix3x4::IDENTITY);
//	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("XZplane");
	outputSlots_[0]->SetVariableName("XZ");
	outputSlots_[0]->SetDescription("XZ Plane Mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
    outputSlots_[1]->SetName("XYplane");
    outputSlots_[1]->SetVariableName("XY");
    outputSlots_[1]->SetDescription("XY Plane Mesh");
    outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
    outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
    
    outputSlots_[2]->SetName("YZplane");
    outputSlots_[2]->SetVariableName("YZ");
    outputSlots_[2]->SetDescription("YZ Plane Mesh");
    outputSlots_[2]->SetVariantType(VariantType::VAR_VARIANTMAP);
    outputSlots_[2]->SetDataAccess(DataAccess::ITEM);
    
}



void Mesh_Plane::SolveInstance(
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
		URHO3D_LOGWARNING("S must be a valid float or integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	float s = inSolveInstance[0].GetFloat();
	

	///////////////////
	// COMPONENT'S WORK

	Variant XZ_planeMesh = MakeXZPlane(s);
    Variant YZ_planeMesh = MakeYZPlane(s);
    Variant XY_planeMesh = MakeXYPlane(s);


	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = XZ_planeMesh;
    outSolveInstance[1] = YZ_planeMesh;
    outSolveInstance[2] = XY_planeMesh;
}
