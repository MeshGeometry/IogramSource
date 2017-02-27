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


#include "Mesh_DecimateMesh.h"

#include <assert.h>

#include "ConversionUtilities.h"
#include "TriMesh.h"

#include <Eigen/Core>
#include <Eigen/Dense>

#pragma warning(push, 0)
#include <igl/decimate.h>
#include <igl/collapse_edge.h>
#include <igl/edge_flaps.h>
#include <igl/remove_unreferenced.h>
#pragma warning(pop)

using namespace Urho3D;

String Mesh_DecimateMesh::iconTexture = "Textures/Icons/Mesh_DecimateMesh.png";

Mesh_DecimateMesh::Mesh_DecimateMesh(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("DecimateMesh");
	SetFullName("Decimate Mesh");
	SetDescription("Perform decimation on triangle mesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh before decimation");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Target Face Count");
	inputSlots_[1]->SetVariableName("C");
	inputSlots_[1]->SetDescription("Target number of faces");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Mesh after decimation");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_DecimateMesh::SolveInstance(
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
	// Verify input slot 1
	//Variant type1 = inSolveInstance[1].GetType();
	//if (type1 != VariantType::VAR_INT) {
	//	URHO3D_LOGWARNING("C must be a valid integer.");
	//	outSolveInstance[0] = Variant();
	//	return;
	//}
	int faceTarget = inSolveInstance[1].GetInt();
	if (faceTarget <= 0) {
		URHO3D_LOGWARNING("C must be > 0.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	bool loadSuccess = IglMeshToMatrices(inMesh, V, F);

	Eigen::MatrixXd Ud;
	Eigen::MatrixXi G;
	bool decimateSuccess = false;
	decimateSuccess = igl::decimate(
		IglFloatToDouble(V),
		F,
		(size_t)faceTarget,
		Ud,
		G
	);
	if (!decimateSuccess) {
		URHO3D_LOGWARNING("Decimate operation failed.");
		outSolveInstance[0] = Variant();
		return;
	}
	Eigen::MatrixXf U = IglDoubleToFloat(Ud);
	Variant outMesh = TriMesh_Make(U, G);

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = outMesh;
}
