#include "ShapeOp_MakeConstraint.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ShapeOp_IogramWrapper.h"

#include "TriMesh.h"

using namespace Urho3D;

namespace {
} // namespace

String ShapeOp_MakeConstraint::iconTexture = "Textures/Icons/DefaultIcon.png";

ShapeOp_MakeConstraint::ShapeOp_MakeConstraint(Context* context) : IoComponentBase(context, 4, 1)
{
	SetName("ShapeOpMakeConstraint");
	SetFullName("ShapeOp Make Constraint");
	SetDescription("...");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh input");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh input");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("ConstraintType");
	inputSlots_[1]->SetVariableName("Type");
	inputSlots_[1]->SetDescription("Constraint type name");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(String("")));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("ConstraintIds");
	inputSlots_[2]->SetVariableName("Ids");
	inputSlots_[2]->SetDescription("Constraint Ids");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDataAccess(DataAccess::LIST);

	inputSlots_[3]->SetName("ConstraintWeight");
	inputSlots_[3]->SetVariableName("Weight");
	inputSlots_[3]->SetDescription("Constraint weight");
	inputSlots_[3]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(Variant(1.0f));
	inputSlots_[3]->DefaultSet();

	outputSlots_[0]->SetName("Constraint");
	outputSlots_[0]->SetVariableName("C");
	outputSlots_[0]->SetDescription("Variant storing constraint data");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void ShapeOp_MakeConstraint::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	// Most of this component's work is input verification
	if (!IsAllInputValid(inSolveInstance)) {
		URHO3D_LOGWARNING("ShapeOp_MakeConstraint --- inputs not of correct VariantType");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	// input 0

	Variant mesh_in = inSolveInstance[0];
	if (!TriMesh_Verify(mesh_in)) {
		URHO3D_LOGWARNING("Mesh input must be a TriMesh");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	VariantVector vertex_list = TriMesh_GetVertexList(mesh_in);
	int nb_points = (int)vertex_list.Size();

	// input 1

	String constraint_type = inSolveInstance[1].GetString();

	// input 2

	VariantVector id_list = inSolveInstance[2].GetVariantVector();
	int nb_ids = (int)id_list.Size();

	// input 3

	float weight = inSolveInstance[3].GetFloat();

	Variant constraint = ShapeOpConstraint_Make(
		constraint_type,
		id_list,
		nb_points,
		weight
	);

	outSolveInstance[0] = constraint;
}