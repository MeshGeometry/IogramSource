#include "Mesh_MeanCurvatureFlow.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "Geomlib_TriMeshMeanCurvatureFlow.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_MeanCurvatureFlow::iconTexture = "";

Mesh_MeanCurvatureFlow::Mesh_MeanCurvatureFlow(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("MeanCurvatureFlow");
	SetFullName("Mean Curvature Flow");
	SetDescription("Mean Curvature Flow on TriMesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh input");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("NumSteps");
	inputSlots_[1]->SetVariableName("Num");
	inputSlots_[1]->SetDescription("Number of steps to flow");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(1));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("N");
	outputSlots_[0]->SetDescription("Mesh output");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_MeanCurvatureFlow::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	if (!IsAllInputValid(inSolveInstance)) {
		SetAllOutputsNull(outSolveInstance);
	}

	// Still need to verify input slot 0
	Variant meshIn = inSolveInstance[0];
	if (!TriMesh_Verify(meshIn)) {
		URHO3D_LOGWARNING("M must be a valid mesh.");
		outSolveInstance[0] = Variant();
		return;
	}

	int num_steps = inSolveInstance[1].GetInt();
	if (num_steps <= 0) {
		URHO3D_LOGWARNING("MeanCurvatureFlow --- Num must be >= 1");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Variant meshOut;
	bool success = Geomlib::TriMesh_MeanCurvatureFlow(meshIn, num_steps, meshOut);
	if (!success) {
		URHO3D_LOGWARNING("MeanCurvatureFlow --- failed on TriMesh");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = meshOut;
}