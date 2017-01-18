#include "Mesh_BoundingBox.h"

#include <Urho3D/Core/Variant.h>

#include "TriMesh.h"

using namespace Urho3D;

String Mesh_BoundingBox::iconTexture = "";

Mesh_BoundingBox::Mesh_BoundingBox(Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("BoundingBox");
	SetFullName("Bounding Box");
	SetDescription("Construct TriMesh bounding box for another TriMesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("BoxMesh");
	outputSlots_[0]->SetVariableName("B");
	outputSlots_[0]->SetDescription("Meshed bounding box of input mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_BoundingBox::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Variant mesh_in = inSolveInstance[0];
	if (!TriMesh_Verify(mesh_in)) {
		URHO3D_LOGWARNING("M must be a TriMesh!");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Variant box_mesh = TriMesh_BoundingBox(mesh_in);

	outSolveInstance[0] = box_mesh;
}