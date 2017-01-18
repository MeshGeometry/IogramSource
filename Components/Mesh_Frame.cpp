#include "Mesh_Frame.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ConversionUtilities.h"
#include "Geomlib_Incenter.h"
#include "Geomlib_TriMeshFrame.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_Frame::iconTexture = "Textures/Icons/Mesh_Frame.png";

Mesh_Frame::Mesh_Frame(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("Frame");
	SetFullName("Mesh Frame");
	SetDescription("Creates an inset frame mesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh to ...");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Inset Factor");
	inputSlots_[1]->SetVariableName("I");
	inputSlots_[1]->SetDescription("Factor controlling amount of inset");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(0.2f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Framed mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_Frame::SolveInstance(
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
	if (!(type == VariantType::VAR_FLOAT)) {
		URHO3D_LOGWARNING("I must be a valid float.");
		outSolveInstance[0] = Variant();
		return;
	}
	float t = inSolveInstance[1].GetFloat();
	if (t <= 0.0f || t >= 1.0f) {
		URHO3D_LOGWARNING("I must be in range 0 < I < 1.");
		outSolveInstance[0] = Variant();
		return;
	}
	
	///////////////////
	// COMPONENT'S WORK

	Variant meshOut;
	bool success = Geomlib::TriMeshFrame(meshIn, t, meshOut);
	if (!success) {
		URHO3D_LOGWARNING("Frame operation failed.");
		outSolveInstance[0] = Variant();
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = meshOut;
}