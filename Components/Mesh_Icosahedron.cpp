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


#include "Mesh_Icosahedron.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ConversionUtilities.h"
#include "StockGeometries.h"
#include "TriMesh.h"
#include "Geomlib_TransformVertexList.h"
#include "Geomlib_TriMeshThicken.h"

using namespace Urho3D;

String Mesh_Icosahedron::iconTexture = "Textures/Icons/Mesh_Icosahedron.png";

Mesh_Icosahedron::Mesh_Icosahedron(Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("Icosahedron");
	SetFullName("ConstructIcosahedron");
	SetDescription("Construct an icosahedron mesh from scale");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("Transformation");
	inputSlots_[0]->SetVariableName("T");
	inputSlots_[0]->SetDescription("Transformation to apply to cube");
	inputSlots_[0]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[0]->SetDefaultValue(Matrix3x4::IDENTITY);
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Icosahedron Mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_Icosahedron::SolveInstance(
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
	if (type0 != VariantType::VAR_MATRIX3X4) {
		URHO3D_LOGWARNING("T must be a valid transform.");
		outSolveInstance[0] = Variant();
		return;
	}
	Matrix3x4 tr = inSolveInstance[0].GetMatrix3x4();

	///////////////////
	// COMPONENT'S WORK

	Variant baseIcosahedron = MakeIcosahedron();
	Variant icosahedron = TriMesh_ApplyTransform(baseIcosahedron, tr);

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = icosahedron;
}