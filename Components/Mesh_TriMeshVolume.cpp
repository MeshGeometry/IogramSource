#include "Mesh_TriMeshVolume.h"

#include <assert.h>

#include <iostream>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>


#include "Geomlib_TriMeshVolume.h"
#include "ConversionUtilities.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_TriMeshVolume::iconTexture = "Textures/Icons/Mesh_TriMeshVolume.png";

Mesh_TriMeshVolume::Mesh_TriMeshVolume(Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("TriMeshVolume");
	SetFullName("TriMesh Volume");
	SetDescription("Compute volume of TriMesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh input to volume computation");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("Volume");
	outputSlots_[0]->SetVariableName("V");
	outputSlots_[0]->SetDescription("Volume of mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_TriMeshVolume::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	Variant tri_mesh = inSolveInstance[0];
	if (!TriMesh_Verify(tri_mesh)) {
		URHO3D_LOGWARNING("M must be a valid mesh.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	float volume = Geomlib::TriMeshVolume(tri_mesh);
	std::cout << "TriMeshVolume = " << volume << "\n";

	URHO3D_LOGINFO("Calculated Mesh Volume: " + String(volume));

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = volume;
}