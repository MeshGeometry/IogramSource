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


#include "Mesh_Remesh.h"

#include <assert.h>

#include <iostream>
#include <vector>

#include <Eigen/Core>

#pragma warning(push, 0)
#include <igl/adjacency_list.h>
#pragma warning(pop)

#include "ConversionUtilities.h"
#include "TriMesh.h"
#include "Geomlib_TriMeshAverageEdgeLength.h"
#include "Geomlib_TriMeshEdgeSplit.h"
#include "Geomlib_TriMeshEdgeCollapse.h"

using namespace Urho3D;

String Mesh_Remesh::iconTexture = "";

Mesh_Remesh::Mesh_Remesh(Context* context) : IoComponentBase(context, 4, 1)
{
	SetName("Remesh");
	SetFullName("Remesh");
	SetDescription("Perform basic remeshing on a TriMesh");

	inputSlots_[0]->SetName("TriMesh");
	inputSlots_[0]->SetVariableName("TriMesh");
	inputSlots_[0]->SetDescription("TriMesh");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("TargetEdgeLength");
	inputSlots_[1]->SetVariableName("Target");
	inputSlots_[1]->SetDescription("Target average edge length (optional)");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[2]->SetName("Tolerance");
	inputSlots_[2]->SetVariableName("Tolerance");
	inputSlots_[2]->SetDescription("Tolerance from average length to split/collapse");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(Variant(0.33f));
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("NumSteps");
	inputSlots_[3]->SetVariableName("NumSteps");
	inputSlots_[3]->SetDescription("Number of split/collapse steps to perform");
	inputSlots_[3]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(Variant(2));
	inputSlots_[3]->DefaultSet();

	outputSlots_[0]->SetName("TriMesh");
	outputSlots_[0]->SetVariableName("TriMesh");
	outputSlots_[0]->SetDescription("TriMesh after remeshing");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_Remesh::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	Variant inMesh = inSolveInstance[0];
	if (!TriMesh_Verify(inMesh)) {
		URHO3D_LOGWARNING("Mesh_Remesh -- invalid TriMesh");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	// get input slot 1 (optional)
	Variant target_var = inSolveInstance[1];
	//float target = Geomlib::TriMeshAverageEdgeLength(inMesh);
	//if (target_var.GetType() == VAR_FLOAT)
	//	target = target_var.GetFloat();

	// Verify input slot 0
	float tol = inSolveInstance[2].GetFloat();
	if (tol < 0.0f || tol >= 1.0f) {
		URHO3D_LOGWARNING("Mesh_Remesh -- Tolerance must be in range 0 <= Tolerance < 1.0f");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	// Verify input slot 1
	int steps = inSolveInstance[3].GetInt();
	if (steps < 0) {
		URHO3D_LOGWARNING("Mesh_Remesh -- NumSteps must be non-negative");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Variant curMesh = inMesh;
	for (int i = 0; i < steps; ++i) {

		float avg_edge_length = Geomlib::TriMeshAverageEdgeLength(curMesh);
		if (target_var.GetType() == VAR_FLOAT)
			avg_edge_length = target_var.GetFloat();
		float split_threshold = (1.0f + tol) * avg_edge_length;
		Variant splitMesh = Geomlib::TriMesh_SplitLongEdges(curMesh, split_threshold);
		//avg_edge_length = Geomlib::TriMeshAverageEdgeLength(splitMesh);
		float collapse_threshold = (1.0f - tol) * avg_edge_length;
		curMesh = Geomlib::TriMesh_CollapseShortEdges(splitMesh, collapse_threshold);
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = curMesh;
}
