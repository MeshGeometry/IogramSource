#include "Mesh_Thicken.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ConversionUtilities.h"
#include "Geomlib_TriMeshThicken.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_Thicken::iconTexture = "Textures/Icons/Mesh_Thicken.png";

Mesh_Thicken::Mesh_Thicken(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("ThickenMesh");
	SetFullName("Thicken Triangle Mesh");
	SetDescription("Perform thickening operation on a triangle mesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh to thicken");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Width");
	inputSlots_[1]->SetVariableName("W");
	inputSlots_[1]->SetDescription("Width to thicken by");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(0.04f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Mesh after thickening");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_Thicken::SolveInstance(
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
	float t = inSolveInstance[1].GetFloat();

	///////////////////
	// COMPONENT'S WORK

	Variant meshOut;
	bool success = Geomlib::TriMeshThicken(meshIn, t, meshOut);
	if (!success) {
		URHO3D_LOGWARNING("Thicken operation failed.");
		outSolveInstance[0] = Variant();
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = meshOut;
}