#include "ShapeOp_IogramWrapper.h"

#include <Urho3D/IO/Log.h>

#include "TriMesh.h"

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

	return Urho3D::Variant(constraint);
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

Urho3D::String ShapeOpConstraint_constraintType(const Urho3D::Variant& constraint)
{
	bool ver = ShapeOpConstraint_Verify(constraint);
	if (!ver) return Urho3D::String("");

	Urho3D::VariantMap var_map = constraint.GetVariantMap();
	return var_map["constraintType"].GetString();
}

std::vector<int> ShapeOpConstraint_ids(const Urho3D::Variant& constraint)
{
	bool ver = ShapeOpConstraint_Verify(constraint);
	if (!ver) return std::vector<int>();

	Urho3D::VariantMap var_map = constraint.GetVariantMap();

	Urho3D::VariantVector id_list = var_map["ids"].GetVariantVector();
	std::vector<int> ids;
	for (unsigned i = 0; i < id_list.Size(); ++i) {
		ids.push_back(id_list[i].GetInt());
	}
	return ids;
}

int ShapeOpConstraint_nb_ids(const Urho3D::Variant& constraint)
{
	bool ver = ShapeOpConstraint_Verify(constraint);
	if (!ver) return 0;

	Urho3D::VariantMap var_map = constraint.GetVariantMap();
	return var_map["nb_ids"].GetInt();
}

double ShapeOpConstraint_weight(const Urho3D::Variant& constraint)
{
	bool ver = ShapeOpConstraint_Verify(constraint);
	if (!ver) return 0.0;

	Urho3D::VariantMap var_map = constraint.GetVariantMap();
	return (double)(var_map["weight"].GetFloat());
}

std::vector<double> ShapeOp_TriMeshToPoints(const Urho3D::Variant& tri_mesh)
{
	if (!TriMesh_Verify(tri_mesh)) {
		return std::vector<double>();
	}

	std::vector<double> pts;
	Urho3D::VariantVector vertex_list = TriMesh_GetVertexList(tri_mesh);
	for (unsigned i = 0; i < vertex_list.Size(); ++i) {
		Urho3D::Vector3 v = vertex_list[i].GetVector3();
		pts.push_back((double)v.x_);
		pts.push_back((double)v.y_);
		pts.push_back((double)v.z_);
	}
	return pts;
}

Urho3D::VariantVector ShapeOp_PointsToVertexList(const std::vector<double>& pts)
{
	if (pts.size() <= 0 || pts.size() % 3 != 0) {
		return Urho3D::VariantVector();
	}

	Urho3D::VariantVector vertex_list;
	for (int i = 0; i < pts.size(); i += 3) {
		Urho3D::Vector3 v((float)pts[i], (float)pts[i + 1], (float)pts[i + 2]);
		vertex_list.Push(v);
	}

	return vertex_list;
}

//////////////
// VertexForce

Urho3D::Variant ShapeOpVertexForce_Make(
	const Urho3D::Vector3& force_on_vertex,
	int vertex_id
)
{
	Urho3D::VariantMap vertex_force;
	vertex_force["type"] = Urho3D::String("ShapeOpVertexForce");
	vertex_force["force"] = force_on_vertex;
	vertex_force["id"] = vertex_id;

	return Urho3D::Variant(vertex_force);
}

bool ShapeOpVertexForce_Verify(const Urho3D::Variant& vertex_force)
{
	if (vertex_force.GetType() != Urho3D::VariantType::VAR_VARIANTMAP) return false;

	Urho3D::VariantMap var_map = vertex_force.GetVariantMap();
	Urho3D::Variant var_type = var_map["type"];
	if (var_type.GetType() != Urho3D::VariantType::VAR_STRING) return false;

	if (var_type.GetString() != "ShapeOpVertexForce") return false;

	return true;
}

int ShapeOpVertexForce_id(const Urho3D::Variant& vertex_force)
{
	bool ver = ShapeOpVertexForce_Verify(vertex_force);
	if (!ver) return -1;

	Urho3D::VariantMap var_map = vertex_force.GetVariantMap();

	int id = var_map["id"].GetInt();

	return id;
}

std::vector<double> ShapeOpVertexForce_force(const Urho3D::Variant& vertex_force)
{
	bool ver = ShapeOpVertexForce_Verify(vertex_force);
	if (!ver) return std::vector<double>();

	Urho3D::VariantMap var_map = vertex_force.GetVariantMap();

	Urho3D::Vector3 v = var_map["force"].GetVector3();

	std::vector<double> force;
	force.push_back((double)v.x_);
	force.push_back((double)v.y_);
	force.push_back((double)v.z_);

	return force;
}