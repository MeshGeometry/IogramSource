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


#include "Mesh_Boundary.h"

#include <assert.h>

#include <vector>

#pragma warning(push, 0)
#include <igl/boundary_loop.h>
#pragma warning(pop)

#include "ConversionUtilities.h"
#include "TriMesh.h"
#include "Polyline.h"

#include <Eigen/Core>
#include <Eigen/Dense>

using namespace Urho3D;

String Mesh_Boundary::iconTexture = "Textures/Icons/Mesh_Boundary.png";

Mesh_Boundary::Mesh_Boundary(Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("MeshBoundary");
	SetFullName("Mesh Boundary");
	SetDescription("Compute boundary of a mesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh with boundary");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("Boundary");
	outputSlots_[0]->SetVariableName("B");
	outputSlots_[0]->SetDescription("Boundary of mesh as polyline");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);
}

void Mesh_Boundary::SolveInstance(
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

	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	bool loadSuccess = IglMeshToMatrices(inMesh, V, F);

	VariantVector loopsOut;

	if (loadSuccess) {
		VariantVector vertexList = TriMesh_GetVertexList(inMesh);
		
		VariantVector loopsOut;
		std::vector<std::vector<int>> loops;
		igl::boundary_loop(F, loops);

		for (int j = 0; j < loops.size(); j++)
		{
			std::vector<int> loop = loops[j];
			VariantVector boundaryVertexList;
			Variant boundary_polyline = Variant();
			for (int i = 0; i < loop.size(); ++i) {
				Vector3 vec = vertexList[loop[i]].GetVector3();
				boundaryVertexList.Push(Variant(vec));
			}

			Vector3 ini_vec = vertexList[loop[0]].GetVector3();
			boundaryVertexList.Push(Variant(ini_vec));
			boundary_polyline = Polyline_Make(boundaryVertexList);

			loopsOut.Push(boundary_polyline);

		}

		outSolveInstance[0] = loopsOut;

	}
	else {
		URHO3D_LOGWARNING("Failed to convert mesh to libigl matrices.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	
}
