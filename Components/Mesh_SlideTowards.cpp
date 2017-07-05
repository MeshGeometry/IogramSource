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


#include "Mesh_SlideTowards.h"

#include <assert.h>

#include <iostream>
#include <vector>

#include <Eigen/Core>

#pragma warning(push, 0)
#include <igl/adjacency_list.h>
#pragma warning(pop)

#include "ConversionUtilities.h"
#include "TriMesh.h"
#include "Geomlib_TriMeshClosestPoint.h"

using namespace Urho3D;

String Mesh_SlideTowards::iconTexture = "";

Mesh_SlideTowards::Mesh_SlideTowards(Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("SlideTowards");
	SetFullName("SlideTowards");
	SetDescription("Slide mesh towards another");

	inputSlots_[0]->SetName("SlidingMesh");
	inputSlots_[0]->SetVariableName("SlidingMesh");
	inputSlots_[0]->SetDescription("SlidingMesh");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("FixedMesh");
	inputSlots_[1]->SetVariableName("FixedMesh");
	inputSlots_[1]->SetDescription("FixedMesh");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[2]->SetName("TargetFactor");
	inputSlots_[2]->SetVariableName("TargetFactor");
	inputSlots_[2]->SetDescription("0 - do not move; 1 - move all the way");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(Variant(0.5f));
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("NewMesh");
	outputSlots_[0]->SetVariableName("NewMesh");
	outputSlots_[0]->SetDescription("Mesh after sliding");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_SlideTowards::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Variant sliding_mesh = inSolveInstance[0];
	if (!TriMesh_Verify(sliding_mesh)) {
		URHO3D_LOGWARNING("Mesh_SlideTowards --- SlidingMesh is invalid TriMesh");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	Variant fixed_mesh = inSolveInstance[1];
	if (!TriMesh_Verify(fixed_mesh)) {
		URHO3D_LOGWARNING("Mesh_SlideTowards --- FixedMesh is invalid TriMesh");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	float target_factor = inSolveInstance[2].GetFloat();
	if (target_factor < 0.0f) {
		target_factor = 0.0f;
	}
	else if (target_factor > 1.0f) {
		target_factor = 1.0f;
	}

	VariantVector closest_points;
	Geomlib::TriMeshPerVertexClosestPoint(sliding_mesh, fixed_mesh, closest_points);

	VariantVector original_verts = TriMesh_GetVertexList(sliding_mesh);
	if (original_verts.Size() != closest_points.Size()) {
		URHO3D_LOGWARNING("Mesh_SlideTowards --- operation failed, original_verts.Size() != closest_points.Size(), exiting");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	VariantVector slid_verts;
	for (int i = 0; i < closest_points.Size(); ++i) {
		Vector3 v =
			(1.0f - target_factor) * original_verts[i].GetVector3()
			+ target_factor * closest_points[i].GetVector3();
		slid_verts.Push(v);
	}

	VariantVector face_list = TriMesh_GetFaceList(sliding_mesh);
	Variant new_mesh = TriMesh_Make(slid_verts, face_list);

	outSolveInstance[0] = new_mesh;
}