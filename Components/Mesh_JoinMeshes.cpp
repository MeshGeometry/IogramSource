#include "Mesh_JoinMeshes.h"

#include <assert.h>

#include <iostream>
#include <vector>

#include <Urho3D/Core/Variant.h>

#include "ConversionUtilities.h"
#include "TriMesh.h"
#include "Geomlib_JoinMeshes.h"

using namespace Urho3D;

String Mesh_JoinMeshes::iconTexture = "Textures/Icons/Mesh_JoinMeshes.png";

Mesh_JoinMeshes::Mesh_JoinMeshes(Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("JoinMeshes");
	SetFullName("Join Meshes");
	SetDescription("Join meshes in a list into a single mesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("MeshList");
	inputSlots_[0]->SetVariableName("ML");
	inputSlots_[0]->SetDescription("List of meshes");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[0]->SetName("JoinMesh");
	outputSlots_[0]->SetVariableName("JM");
	outputSlots_[0]->SetDescription("Joined mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_JoinMeshes::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	Variant mesh_list_var = inSolveInstance[0];
	if (mesh_list_var.GetType() != VariantType::VAR_VARIANTVECTOR) {
		URHO3D_LOGWARNING("ML must be a list");
		outSolveInstance[0] = Variant();
		return;
	}
	VariantVector mesh_list = mesh_list_var.GetVariantVector();
	VariantVector actual_mesh_list;
	for (int i = 0; i < mesh_list.Size(); ++i) {
		if (TriMesh_Verify(mesh_list[i])) {
			actual_mesh_list.Push(mesh_list[i]);
		}
	}

	if (actual_mesh_list.Size() == 0) {
		URHO3D_LOGWARNING("ML contains no valid meshes, skipping JoinMeshes operation");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Variant joined_mesh = Geomlib::JoinMeshes(actual_mesh_list);

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = joined_mesh;
}