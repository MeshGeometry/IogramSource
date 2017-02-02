#include "ShapeOp_MakeConstraint.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ShapeOp_API.h"

#include "TriMesh.h"

using namespace Urho3D;

namespace {

bool VerifyConstraintType(const String& constraint_type)
{
	bool ret = false;

	if (constraint_type == "EdgeStrain") {
		ret = true;
	}
	else if (constraint_type == "TriangleStrain") {
		ret = true;
	}
	else if (constraint_type == "TetrahedronStrain") {
		ret = true;
	}
	else if (constraint_type == "Area") {
		ret = true;
	}
	else if (constraint_type == "Volume") {
		ret = true;
	}
	else if (constraint_type == "Bending") {
		ret = true;
	}
	else if (constraint_type == "Closeness") {
		ret = true;
	}
	else if (constraint_type == "Line") {
		ret = true;
	}
	else if (constraint_type == "Plane") {
		//
	}
	else if (constraint_type == "Circle") {
		ret = true;
	}
	else if (constraint_type == "Sphere") {
		ret = true;
	}
	else if (constraint_type == "Similarity") {
		ret = true;
	}
	else if (constraint_type == "Rigid") {
		ret = true;
	}
	else if (constraint_type == "Rectangle") {
		ret = true;
	}
	else if (constraint_type == "Parallelogram") {
		ret = true;
	}
	else if (constraint_type == "Laplacian") {
		ret = true;
	}
	else if (constraint_type == "LaplacianDisplacement") {
		ret = true;
	}
	else if (constraint_type == "Angle") {
		ret = true;
	}

	return ret;
}

bool VerifyConstraintIds(
	const VariantVector& ids_list,
	int nb_ids,
	int nb_points
)
{
	bool ret = true;

	if (!(nb_ids > 0)) {
		URHO3D_LOGWARNING("ShapeOp_MakeConstraint --- ConstraintIds is empty");
		ret = false;
	}
	for (unsigned i = 0; i < nb_ids; ++i) {

		// make sure all ids are VAR_INT
		if (ids_list[i].GetType() != VariantType::VAR_INT) {
			URHO3D_LOGWARNING("ShapeOp_MakeConstraint --- ConstraintIds must all be VAR_INT");
			ret = false;
		}

		// make sure all ids are in valid range
		int id = ids_list[i].GetInt();
		if (id < 0 || id >= nb_points) {
			URHO3D_LOGWARNING("ShapeOp_MakeConstraint --- ConstraindIds has id out of range");
			ret = false;
		}

		// make sure no repeated ids
		for (unsigned j = 0; j < i; ++j) {

			ids_list[j].GetInt() == ids_list[i].GetInt();
			URHO3D_LOGWARNING("ShapeOp_MakeConstraint --- ConstraintIds has repeated id");
			ret = false;
			break;
		}

		if (ret == false) {
			break;
		}
	}

	return ret;
}

bool VerifyConstraint(
	const String& constraint_type,
	int nb_ids
)
{
	bool ret = false;

	if (constraint_type == "EdgeStrain" && nb_ids == 2) {
		ret = true;
	}
	else if (constraint_type == "TriangleStrain" && nb_ids == 3) {
		ret = true;
	}
	else if (constraint_type == "TetrahedronStrain" && nb_ids == 4) {
		ret = true;
	}
	else if (constraint_type == "Area" && nb_ids == 3) {
		ret = true;
	}
	else if (constraint_type == "Volume" && nb_ids == 4) {
		ret = true;
	}
	else if (constraint_type == "Bending" && nb_ids == 4) {
		// indices form two neighboring triangles sharing an edge
		ret = true;
	}
	else if (constraint_type == "Closeness" && nb_ids == 1) {
		ret = true;
	}
	else if (constraint_type == "Line" && nb_ids >= 2) {
		ret = true;
	}
	else if (constraint_type == "Plane" && nb_ids >= 3) {
		//
	}
	else if (constraint_type == "Circle" && nb_ids >= 3) {
		ret = true;
	}
	else if (constraint_type == "Sphere" && nb_ids >= 4) {
		ret = true;
	}
	else if (constraint_type == "Similarity" && nb_ids >= 1) {
		ret = true;
	}
	else if (constraint_type == "Rigid" && nb_ids >= 1) {
		ret = true;
	}
	else if (constraint_type == "Rectangle" && nb_ids == 4) {
		ret = true;
	}
	else if (constraint_type == "Parallelogram" && nb_ids == 4) {
		ret = true;
	}
	else if (constraint_type == "Laplacian" && nb_ids >= 2) {
		// center vertex first, then the one ring neighborhood
		ret = true;
	}
	else if (constraint_type == "LaplacianDisplacement" && nb_ids >= 2) {
		// center vertex_first, then the one ring neighborhood
		ret = true;
	}
	else if (constraint_type == "Angle" && nb_ids == 3) {
		ret = true;
	}

	return ret;
}

}

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
	inputSlots_[3]->SetDefaultValue(Variant(String("")));
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

	// Verify mesh input

	Variant mesh_in = inSolveInstance[0];
	if (!TriMesh_Verify(mesh_in)) {
		URHO3D_LOGWARNING("Mesh input must be a TriMesh");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	VariantVector vertex_list = TriMesh_GetVertexList(mesh_in);
	int nb_points = (int)vertex_list.Size();
	if (nb_points <= 0) {
		URHO3D_LOGWARNING("ShapeOp_MakeConstraint --- Mesh must have positive number of points");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	// Verify constraint type input

	String constraint_type = inSolveInstance[1].GetString();

	// Verify ids input

	VariantVector ids_list = inSolveInstance[2].GetVariantVector();
	int nb_ids = (int)ids_list.Size();
	bool ids_valid = VerifyConstraintIds(ids_list, nb_ids, nb_points);

	if (ids_valid == false) {
		URHO3D_LOGWARNING("ShapeOp_MakeConstraint --- ConstraintIds invalid, see console warning above");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	bool constraint_valid = VerifyConstraint(constraint_type, nb_ids);

	if (constraint_valid == false) {
		URHO3D_LOGWARNING("ShapeOp_MakeConstraint --- ConstraintType & ConstraintIds not valid together");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	// Verify weight

	inSolveInstance[3].GetFloat();
	double weight = 0.8;

	// constraint is valid

	VariantMap constraint;
	constraint["constraintType"] = constraint_type;
	constraint["ids"] = ids_list;
	constraint["nb_ids"] = nb_ids;
	constraint["weight"] = weight;

	outSolveInstance[0] = constraint;
}