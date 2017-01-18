#include "Mesh_Offset.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ConversionUtilities.h"
#include "Geomlib_TriMeshOffset.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_Offset::iconTexture = "Textures/Icons/Mesh_Offset.png";

Mesh_Offset::Mesh_Offset(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("OffsetMesh");
	SetFullName("Offset Triangle Mesh");
	SetDescription("Perform offset operation on a triangle mesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh to offset");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Width");
	inputSlots_[1]->SetVariableName("W");
	inputSlots_[1]->SetDescription("Width to offset by");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(1.0f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Mesh after offsetting");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_Offset::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	Variant meshIn = inSolveInstance[0];
	if (!TriMesh_Verify(meshIn)) {
		URHO3D_LOGWARNING("M must be a valid mesh.");
		outSolveInstance[0] = Variant();
		return;
	}
	// Verify input slot 1
	VariantType type = inSolveInstance[1].GetType();
	if (!(type == VariantType::VAR_FLOAT || type == VariantType::VAR_INT)) {
		URHO3D_LOGWARNING("W must be a valid float.");
		outSolveInstance[0] = Variant();
		return;
	}
	float t = inSolveInstance[1].GetFloat();

	///////////////////
	// COMPONENT'S WORK

	Variant meshOut;
	bool success = Geomlib::TriMeshOffset(meshIn, t, meshOut);
	if (!success) {
		URHO3D_LOGWARNING("Offset operation failed.");
		outSolveInstance[0] = Variant();
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = meshOut;
}