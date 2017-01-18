#include "Mesh_SmoothMesh.h"

#include <assert.h>

#include <iostream>
#include <vector>

#include <Eigen/Core>

#pragma warning(push, 0)
#include <igl/adjacency_list.h>
#pragma warning(pop)

#include "ConversionUtilities.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_SmoothMesh::iconTexture = "Textures/Icons/Mesh_SmoothMesh.png";

Mesh_SmoothMesh::Mesh_SmoothMesh(Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("SmoothMesh");
	SetFullName("Smooth Mesh");
	SetDescription("Perform smoothing on triangle mesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh before smoothing");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Iterations");
	inputSlots_[1]->SetVariableName("I");
	inputSlots_[1]->SetDescription("Number of smoothing iterations");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(1));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Target Factor");
	inputSlots_[2]->SetVariableName("F");
	inputSlots_[2]->SetDescription("Move towards smoothed target by factor");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(Variant(0.5f));
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Mesh after smoothing");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_SmoothMesh::SolveInstance(
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
	VariantType type1 = inSolveInstance[1].GetType();
	if (type1 != VariantType::VAR_INT) {
		URHO3D_LOGWARNING("I must be a valid integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	int steps = inSolveInstance[1].GetInt();
	if (steps < 0) {
		URHO3D_LOGWARNING("S must be a non-negative integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	// Verify input slot 2
	VariantType type2 = inSolveInstance[2].GetType();
	if (type2 != VariantType::VAR_FLOAT) {
		URHO3D_LOGWARNING("F must be a valid float.");
		outSolveInstance[0] = Variant();
		return;
	}
	float targetFactor = inSolveInstance[2].GetFloat();
	if (targetFactor < 0.0f || targetFactor > 1.0f) {
		URHO3D_LOGWARNING("F must be in range 0 <= F <= 1.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	bool loadSuccess = IglMeshToMatrices(inMesh, V, F);
	Variant outMesh;

	if (loadSuccess) {
		Eigen::MatrixXf W = V;

		std::vector<std::vector<unsigned> > adj;
		igl::adjacency_list(F, adj);

		for (int index = 0; index < steps; ++index) {
			for (unsigned i = 0; i < adj.size(); ++i) {
				std::vector<unsigned> cur_adj = adj[i];
				Eigen::RowVector3f v(0.0f, 0.0f, 0.0f);
				for (unsigned j = 0; j < cur_adj.size(); ++j) {
					v += W.row(cur_adj[j]);
				}
				if (cur_adj.size() > 0) {
					float scalar = 1.0f / cur_adj.size();
					v = scalar * v;
				}
				W.row(i) = v;
			}
		}

		for (unsigned i = 0; i < V.rows(); ++i) {
			V.row(i) = (1.0f - targetFactor) * V.row(i) + targetFactor * W.row(i);
		}
		//outMesh = IglMatricesToMesh(V, F);
		outMesh = TriMesh_Make(V, F);
	}
	else {
		URHO3D_LOGWARNING("Smooth operation failed.");
		outSolveInstance[0] = Variant();
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = outMesh;
}
