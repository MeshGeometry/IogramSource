#include "Mesh_MeshPlaneIntersection.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ConversionUtilities.h"
#include "TriMesh.h"
#include "Geomlib_TriMeshPlaneIntersection.h"

using namespace Urho3D;

String Mesh_MeshPlaneIntersection::iconTexture = "Textures/Icons/Mesh_MeshPlaneIntersection.png";

Mesh_MeshPlaneIntersection::Mesh_MeshPlaneIntersection(Context* context) : IoComponentBase(context, 3, 2)
{
	SetName("MeshPlaneIntersection");
	SetFullName("Mesh Plane Intersection");
	SetDescription("Intersect triangle mesh with plane");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh to intersect with plane");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Point");
	inputSlots_[1]->SetVariableName("P");
	inputSlots_[1]->SetDescription("Point on plane");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Vector3(0.0f, 0.0f, 0.0f));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Normal");
	inputSlots_[2]->SetVariableName("N");
	inputSlots_[2]->SetDescription("Normal to plane");
	inputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(Vector3(1.0f, 0.75f, 0.0f));
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Mesh1");
	outputSlots_[0]->SetVariableName("M1");
	outputSlots_[0]->SetDescription("Mesh on normal side of plane");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Mesh2");
	outputSlots_[1]->SetVariableName("M2");
	outputSlots_[1]->SetDescription("Mesh on non-normal side of plane");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_MeshPlaneIntersection::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT
	Variant mesh = inSolveInstance[0];
	Vector3 point = inSolveInstance[1].GetVector3();
	Vector3 normal = inSolveInstance[2].GetVector3();

	Variant mesh_normal_side, mesh_non_normal_side;

	bool success = Geomlib::TriMeshPlaneIntersection(
		mesh, point, normal, mesh_normal_side, mesh_non_normal_side
	);

	if (success) {
		outSolveInstance[0] = mesh_normal_side;
		outSolveInstance[1] = mesh_non_normal_side;
	}

}
