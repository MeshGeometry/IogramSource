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


#include "Mesh_HausdorffDistance.h"

#include <Urho3D/Core/Variant.h>

#include "TriMesh.h"
#include "Geomlib_HausdorffDistance.h"

using namespace Urho3D;

String Mesh_HausdorffDistance::iconTexture = "";

Mesh_HausdorffDistance::Mesh_HausdorffDistance(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("HausdorffDistance");
	SetFullName("Hausdorff Distance");
	SetDescription("Compute Hausdorff distance between TriMeshes");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh1");
	inputSlots_[0]->SetVariableName("M1");
	inputSlots_[0]->SetDescription("First mesh");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Mesh2");
	inputSlots_[1]->SetVariableName("M2");
	inputSlots_[1]->SetDescription("Second mesh");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("HausdorffDistance");
	outputSlots_[0]->SetVariableName("HD");
	outputSlots_[0]->SetDescription("Hausdorff distance between input meshes");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_HausdorffDistance::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Variant mesh1 = inSolveInstance[0];
	if (!TriMesh_Verify(mesh1)) {
		URHO3D_LOGWARNING("M1 must be a TriMesh!");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	Variant mesh2 = inSolveInstance[1];
	if (!TriMesh_Verify(mesh2)) {
		URHO3D_LOGWARNING("M2 must be a TriMesh!");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	float distance;
	bool success = Geomlib::TriMesh_HausdorffDistance(mesh1, mesh2, distance);
	if (success) {
		outSolveInstance[0] = distance;
	}
	else {
		URHO3D_LOGWARNING("Hausdorff distance calculation failed....");
		SetAllOutputsNull(outSolveInstance);
	}
}