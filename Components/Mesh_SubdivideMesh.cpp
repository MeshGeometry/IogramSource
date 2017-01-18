#include "Mesh_SubdivideMesh.h"

#include <assert.h>

#include <iostream>
#include <vector>

#include <Urho3D/Core/Variant.h>

#include "Geomlib_TriMeshSubdivide.h"

#include "ConversionUtilities.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_SubdivideMesh::iconTexture = "Textures/Icons/Mesh_SubdivideMesh.png";

Mesh_SubdivideMesh::Mesh_SubdivideMesh(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("SubdivideMesh");
	SetFullName("Subdivide Triangle Mesh");
	SetDescription("Perform subdivision on triangle mesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh before subdivision");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Steps");
	inputSlots_[1]->SetVariableName("S");
	inputSlots_[1]->SetDescription("Number of subdivision steps");
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

void Mesh_SubdivideMesh::SolveInstance(
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
	VariantType type = inSolveInstance[1].GetType();
	if (type != VariantType::VAR_INT) {
		URHO3D_LOGWARNING("S must be a valid integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	int steps = inSolveInstance[1].GetInt();
	if (steps < 0) {
		URHO3D_LOGWARNING("S must be a non-negative integer.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Variant outMesh;
	bool success = Geomlib::TriMeshSubdivide(inMesh, steps, outMesh);
	if (!success) {
		URHO3D_LOGWARNING("Subdivide operation failed.");
		outSolveInstance[0] = Variant();
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = outMesh;
}