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


#include "Mesh_CollapseShortEdges.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ConversionUtilities.h"
#include "Geomlib_TriMeshAverageEdgeLength.h"
#include "Geomlib_TriMeshEdgeCollapse.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_CollapseShortEdges::iconTexture = "";

Mesh_CollapseShortEdges::Mesh_CollapseShortEdges(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("CollapseShortEdges");
	SetFullName("Collapse Short Edges");
	SetDescription("Collapse edges shorter than (1 - tol) * avg");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("TriMesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("TriMesh before edge collapses");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Tol");
	inputSlots_[1]->SetVariableName("T");
	inputSlots_[1]->SetDescription("Collapse edges shorter than (1 - tol) * avg");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(0.4f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("TriMesh");
	outputSlots_[0]->SetVariableName("N");
	outputSlots_[0]->SetDescription("TriMesh after edge collapses");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_CollapseShortEdges::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	///////////////////
	// VERIFY & EXTRACT

	if (!IsAllInputValid(inSolveInstance)) {
		SetAllOutputsNull(outSolveInstance);
	}

	// Still need to verify input slot 0
	Variant meshIn = inSolveInstance[0];
	if (!TriMesh_Verify(meshIn)) {
		URHO3D_LOGWARNING("M must be a valid TriMesh.");
		outSolveInstance[0] = Variant();
		return;
	}
	float tol = inSolveInstance[1].GetFloat();

	///////////////////
	// COMPONENT'S WORK

	float L = (1.0f - tol) * Geomlib::TriMeshAverageEdgeLength(meshIn);
	Variant meshOut = Geomlib::TriMesh_CollapseShortEdges(meshIn, L);

	if (!TriMesh_Verify(meshOut)) {
		URHO3D_LOGWARNING("CollapseShortEdges failed on TriMesh, check health of mesh");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = meshOut;
}