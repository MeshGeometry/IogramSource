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


#include "Mesh_Torus.h"

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

String Mesh_Torus::iconTexture = "Textures/Icons/Mesh_Torus.png";

Mesh_Torus::Mesh_Torus(Context* context) : IoComponentBase(context, 5, 2)
{
	SetName("Torus");
	SetFullName("ConstructTorusMesh");
	SetDescription("Construct a torus mesh from radii");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("OuterRadius");
	inputSlots_[0]->SetVariableName("O");
	inputSlots_[0]->SetDescription("Outer radius");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant(2.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("InnerRadius");
	inputSlots_[1]->SetVariableName("I");
	inputSlots_[1]->SetDescription("Inner radius");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(2.0f));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("FirstPower");
	inputSlots_[2]->SetVariableName("P1");
	inputSlots_[2]->SetDescription("FirstPower");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(Variant(1.0f));
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("SecondPower");
	inputSlots_[3]->SetVariableName("P2");
	inputSlots_[3]->SetDescription("SecondPower");
	inputSlots_[3]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(Variant(1.0f));
	inputSlots_[3]->DefaultSet();

	inputSlots_[4]->SetName("MeshResolution");
	inputSlots_[4]->SetVariableName("R");
	inputSlots_[4]->SetDescription("Integer (>3) describing mesh resolution");
	inputSlots_[4]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[4]->SetDefaultValue(Variant(8));
	inputSlots_[4]->DefaultSet();

	//inputSlots_[4]->SetName("Transformation");
	//inputSlots_[4]->SetVariableName("T");
	//inputSlots_[4]->SetDescription("Transformation to apply to cube");
	//inputSlots_[4]->SetVariantType(VariantType::VAR_MATRIX3X4);
	//inputSlots_[4]->SetDefaultValue(Matrix3x4::IDENTITY);
	//inputSlots_[4]->DefaultSet();

	outputSlots_[0]->SetName("QuadMesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Torus QuadMesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
    outputSlots_[1]->SetName("TriMesh");
    outputSlots_[1]->SetVariableName("T");
    outputSlots_[1]->SetDescription("Torus TriMesh");
    outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
    outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_Torus::SolveInstance(
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
		URHO3D_LOGWARNING("Outer radius must be a valid float or integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	float outer = inSolveInstance[0].GetFloat();
	if (outer <= 0.0f) {
		URHO3D_LOGWARNING("Outer radius must be > 0.");
		outSolveInstance[0] = Variant();
		return;
	}

	//verify input slot 1
	VariantType type1 = inSolveInstance[1].GetType();
	if (!(type1 == VariantType::VAR_FLOAT || type1 == VariantType::VAR_INT)) {
		URHO3D_LOGWARNING("Inner radius must be a valid float or integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	float inner = inSolveInstance[1].GetFloat();
	if (inner <= 0.0f) {
		URHO3D_LOGWARNING("Inner radius must be > 0.");
		outSolveInstance[0] = Variant();
		return;
	}

	// Verify input slot 2
	VariantType type2 = inSolveInstance[2].GetType();
	if (!(type2 == VariantType::VAR_INT || type2 == VariantType::VAR_FLOAT)) {
		URHO3D_LOGWARNING("First power must be a valid integer or float.");
		outSolveInstance[0] = Variant();
		return;
	}
	float p_1 = inSolveInstance[2].GetFloat();
	if (p_1 <= 0.2f || p_1 > 4.0f) {
		URHO3D_LOGWARNING("First power must be between 0.2 and 4.0");
		outSolveInstance[0] = Variant();
		return;
	}

	// Verify input slot 3
	VariantType type3 = inSolveInstance[3].GetType();
	if (!(type3 == VariantType::VAR_INT || type3 == VariantType::VAR_FLOAT)) {
		URHO3D_LOGWARNING("Second power must be a valid integer or float.");
		outSolveInstance[0] = Variant();
		return;
	}
	float p_2 = inSolveInstance[3].GetFloat();
	if (p_2 <= 0.2f || p_2 > 4.0f) {
		URHO3D_LOGWARNING("Second power must be between 0.2 and 4.0");
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
	VariantType type4 = inSolveInstance[4].GetType();
	if (type4 != VariantType::VAR_INT) {
		URHO3D_LOGWARNING("R must be a valid int!");
		outSolveInstance[0] = Variant();
		return;
	}
	int res = inSolveInstance[4].GetInt();
	if (res < 3) {
		URHO3D_LOGWARNING("R must be larger than 3");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

    Variant torusTriMesh;
	Variant torusMesh = MakeSuperTorus(torusTriMesh, outer, inner, p_1, p_2, res);

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = torusMesh;
    outSolveInstance[1] = torusTriMesh;
}
