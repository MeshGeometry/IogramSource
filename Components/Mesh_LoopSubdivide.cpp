#include "Mesh_LoopSubdivide.h"

#include <assert.h>

#include <iostream>
#include <vector>

#include <Urho3D/Core/Variant.h>

#include "Geomlib_TriMeshLoopSubdivide.h"

#include "ConversionUtilities.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_LoopSubdivide::iconTexture = "Textures/Icons/Mesh_LoopSubdivide.png";

Mesh_LoopSubdivide::Mesh_LoopSubdivide(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("LoopSubdivide");
	SetFullName("Loop Subdivide Triangle Mesh");
	SetDescription("Perform loop subdivision on triangle mesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh before subdivision");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Iterations");
	inputSlots_[1]->SetVariableName("I");
	inputSlots_[1]->SetDescription("Number of iterations to perform");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(1));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Mesh after subdivision");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_LoopSubdivide::SolveInstance(
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
	int iterations = inSolveInstance[1].GetInt();
	if (iterations < 0) {
		URHO3D_LOGWARNING("I must be a non-negative integer.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Variant outMesh;

	for (int i = 0; i < iterations; ++i) {
		bool success = Geomlib::TriMeshLoopSubdivide(inMesh, 1, outMesh);
		if (!success) {
			URHO3D_LOGWARNING("Loop subdivision operation failed.");
			outSolveInstance[0] = Variant();
			return;
		}
		inMesh = outMesh;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = outMesh;
}