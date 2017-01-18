#include "Mesh_SplitLongEdges.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ConversionUtilities.h"
#include "Geomlib_TriMeshAverageEdgeLength.h"
#include "Geomlib_TriMeshEdgeSplit.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_SplitLongEdges::iconTexture = "";

Mesh_SplitLongEdges::Mesh_SplitLongEdges(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("SplitLongEdges");
	SetFullName("Split Long Edges");
	SetDescription("Split edges longer than (1 + tol) * avg");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("TriMesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("TriMesh before edge splits");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Tol");
	inputSlots_[1]->SetVariableName("T");
	inputSlots_[1]->SetDescription("Split edges longer than (1 + tol) * avg");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(0.4f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("TriMesh");
	outputSlots_[0]->SetVariableName("N");
	outputSlots_[0]->SetDescription("TriMesh after edge splits");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_SplitLongEdges::SolveInstance(
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

	float L = (1.0f + tol) * Geomlib::TriMeshAverageEdgeLength(meshIn);
	Variant meshOut = Geomlib::TriMesh_SplitLongEdges(meshIn, L);

	if (!TriMesh_Verify(meshOut)) {
		URHO3D_LOGWARNING("SplitLongEdges failed on TriMesh, check health of mesh");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = meshOut;
}