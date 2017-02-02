#include "ShapeOp_IogramWrapper.h"

#include <Urho3D/IO/Log.h>

namespace {

bool IsValidShapeOpConstraintType(const Urho3D::String& constraint_type)
{
	if (
		constraint_type == Urho3D::String("EdgeStrain") ||
		constraint_type == Urho3D::String("TriangleStrain") ||
		constraint_type == Urho3D::String("TetrahedronStrain") ||
		constraint_type == Urho3D::String("Area") ||
		constraint_type == Urho3D::String("Volume") ||
		constraint_type == Urho3D::String("Bending") ||
		constraint_type == Urho3D::String("Closeness") ||
		constraint_type == Urho3D::String("Line") ||
		constraint_type == Urho3D::String("Plane") ||
		constraint_type == Urho3D::String("Circle") ||
		constraint_type == Urho3D::String("Sphere") ||
		constraint_type == Urho3D::String("Similarity") ||
		constraint_type == Urho3D::String("Rigid") ||
		constraint_type == Urho3D::String("Rectangle") ||
		constraint_type == Urho3D::String("Parallelogram") ||
		constraint_type == Urho3D::String("Laplacian") ||
		constraint_type == Urho3D::String("LaplacianDisplacement") ||
		constraint_type == Urho3D::String("Angle")
		)
	{
		return true;
	}

	return false;
}

bool IsValidShapeOpIdList(
	const Urho3D::VariantVector& id_list,
	int nb_points
)
{
	if (id_list.Size() <= 0) {
		URHO3D_LOGWARNING("IsValidConstraintData --- id_list.Size() <= 0");
		return false;
	}

	for (unsigned i = 0; i < id_list.Size(); ++i) {

		// make sure Variant stores VAR_INT
		if (id_list[i].GetType() != Urho3D::VariantType::VAR_INT) {
			URHO3D_LOGWARNING("IsValidConstraintData --- id_list[i].GetType() != VAR_INT");
			return false;
		}

		// make sure id is in valid range
		int id = id_list[i].GetInt();
		if (id < 0 || id >= nb_points) {
			URHO3D_LOGWARNING("IsValidConstraintData --- id < 0 || id >= nb_points");
			return false;
		}

		// make sure no repeated ids
		for (unsigned j = 0; j < i; ++j) {

			if (id_list[j].GetInt() == id_list[i].GetInt()) {
				URHO3D_LOGWARNING("IsValidConstraintData --- id_list[j].GetInt() == id_list[i].GetInt()");
				return false;
			}
		}
	}
	return true;
}

}

bool IsValidShapeOpConstraint(
	const Urho3D::String& constraint_type,
	const Urho3D::VariantVector& id_list,
	int nb_points,
	float weight
)
{
	if (!IsValidShapeOpConstraintType(constraint_type)) {
		return false;
	}

	if (!IsValidShapeOpIdList(id_list, nb_points)) {
		return false;
	}

	if (nb_points <= 0) {
		URHO3D_LOGWARNING("IsValidShapeOpConstraint --- nb_points <= 0");
		return false;
	}

	if (weight <= 0.0f) {
		URHO3D_LOGWARNING("IsValidShapeOpConstraint --- weight <= 0.0f");
		return false;
	}

	int nb_ids = (int)id_list.Size();

	if (constraint_type == "EdgeStrain" && nb_ids == 2) {
		return true;
	}
	else if (constraint_type == "TriangleStrain" && nb_ids == 3) {
		return true;
	}
	else if (constraint_type == "TetrahedronStrain" && nb_ids == 4) {
		return true;
	}
	else if (constraint_type == "Area" && nb_ids == 3) {
		return true;
	}
	else if (constraint_type == "Volume" && nb_ids == 4) {
		return true;
	}
	else if (constraint_type == "Bending" && nb_ids == 4) {
		return true;
	}
	else if (constraint_type == "Closeness" && nb_ids == 1) {
		return true;
	}
	else if (constraint_type == "Line" && nb_ids >= 2) {
		return true;
	}
	else if (constraint_type == "Plane" && nb_ids >= 3) {
		return true;
	}
	else if (constraint_type == "Circle" && nb_ids >= 3) {
		return true;
	}
	else if (constraint_type == "Sphere" && nb_ids >= 4) {
		return true;
	}
	else if (constraint_type == "Similarity" && nb_ids >= 1) {
		return true;
	}
	else if (constraint_type == "Rigid" && nb_ids >= 1) {
		return true;
	}
	else if (constraint_type == "Rectangle" && nb_ids == 4) {
		return true;
	}
	else if (constraint_type == "Parallelogram" && nb_ids == 4) {
		return true;
	}
	else if (constraint_type == "Laplacian" && nb_ids >= 2) {
		return true;
	}
	else if (constraint_type == "LaplacianDisplacement" && nb_ids >= 2) {
		return true;
	}
	else if (constraint_type == "Angle" && nb_ids == 3) {
		return true;
	}
	else {
		URHO3D_LOGWARNING("IsValidShapeOpConstraint --- incompatible constraint_type and id_list");
		return false;
	}
}

Urho3D::Variant ShapeOpConstraint_Make(
	const Urho3D::String& constraint_type,
	const Urho3D::VariantVector& id_list,
	int nb_points,
	float weight
)
{
	if (!IsValidShapeOpConstraint(constraint_type, id_list, nb_points, weight)) {
		URHO3D_LOGWARNING("ShapeOpConstraintMake --- invalid ShapeOpConstraint, returning VAR_NONE");
		return Urho3D::Variant();
	}

	int nb_ids = id_list.Size();

	Urho3D::VariantMap constraint;
	constraint["type"] = Urho3D::String("ShapeOpConstraint");
	constraint["constraintType"] = constraint_type;
	constraint["ids"] = id_list;
	constraint["nb_ids"] = nb_ids;
	constraint["weight"] = weight;

	return constraint;
}

bool ShapeOpConstraint_Verify(const Urho3D::Variant& constraint)
{
	if (constraint.GetType() != Urho3D::VariantType::VAR_VARIANTMAP) return false;

	Urho3D::VariantMap var_map = constraint.GetVariantMap();
	Urho3D::Variant var_type = var_map["type"];
	if (var_type.GetType() != Urho3D::VariantType::VAR_STRING) return false;

	if (var_type.GetString() != "ShapeOpConstraint") return false;

	return true;
}