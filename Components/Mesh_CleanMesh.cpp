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


#include "Mesh_CleanMesh.h"

#include <assert.h>

#pragma warning(push, 0)
#include <igl/remove_unreferenced.h>
#pragma warning(pop)

#include "ConversionUtilities.h"
#include "TriMesh.h"

#include <Eigen/Core>
#include <Eigen/Dense>

using namespace Urho3D;

String Mesh_CleanMesh::iconTexture = "Textures/Icons/Mesh_CleanMesh.png";

Mesh_CleanMesh::Mesh_CleanMesh(Context* context) : IoComponentBase(context, 1, 2)
{
	SetName("CleanMeshVertices");
	SetFullName("Cull Unused Vertices");
	SetDescription("Cull unused vertices from trimesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh with unused vertices");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Mesh after removing unused vertices");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("NumRemoved");
	outputSlots_[1]->SetVariableName("N");
	outputSlots_[1]->SetDescription("Number of vertices removed");
	outputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_CleanMesh::SolveInstance(
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
		URHO3D_LOGWARNING("M must be a valid mesh.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Eigen::MatrixXf V, NV;
	Eigen::MatrixXi F, NF;
	bool loadSuccess = IglMeshToMatrices(inMesh, V, F);
	if (!loadSuccess) {
		URHO3D_LOGWARNING("Failed to produce Eigen matrices V, F from trimesh M");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Eigen::VectorXi _1;
	igl::remove_unreferenced(V, F, NV, NF, _1);
	Variant out_mesh = TriMesh_Make(NV, NF);
	int num_removed = V.rows() - NV.rows();

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = out_mesh;
	outSolveInstance[1] = Variant(num_removed);
}