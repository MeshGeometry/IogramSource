#include "ShapeOp_IogramWrapper.h"

#include <Urho3D/IO/Log.h>

#include <iostream>

#include "TriMesh.h"

namespace {

} // namespace

////////////////////
// ShapeOpConstraint

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

double ShapeOpConstraint_weight(const Urho3D::Variant& constraint)
{
	bool ver = ShapeOpConstraint_Verify(constraint);
	if (!ver) return 0.0;

	Urho3D::VariantMap var_map = constraint.GetVariantMap();
	return (double)(var_map["weight"].GetFloat());
}

////////////////
// ShapeOpVertex

Urho3D::Variant ShapeOpVertex_Make(
	const Urho3D::Vector3& v
)
{
	Urho3D::VariantMap var_map;
	var_map["coords"] = Urho3D::Variant(v);
	var_map["raw_index"] = Urho3D::Variant(-1);
	var_map["processed_index"] = Urho3D::Variant(-1);

	return Urho3D::Variant(var_map);
}

void SetConstraintRawIndex(
	Urho3D::Variant& constraint,
	int i, // index into constraint's "vertices" VariantVector of ShapeOpVertex objects
	int raw_index // index into master raw vertex list to store
)
{
	Urho3D::VariantMap* var_map = constraint.GetVariantMapPtr();

	Urho3D::VariantVector* vertices = (*var_map)["vertices"].GetVariantVectorPtr();
	Urho3D::VariantMap* shapeop_vertex_var_map = (*vertices)[i].GetVariantMapPtr();
	(*shapeop_vertex_var_map)["raw_index"] = Urho3D::Variant(raw_index);
}

int GetConstraintRawIndex(
	Urho3D::Variant& constraint,
	int i // index into constraint's "vertices" VariantVector of ShapeOpVertex objects
)
{
	Urho3D::VariantMap* var_map = constraint.GetVariantMapPtr();

	Urho3D::VariantVector* vertices = (*var_map)["vertices"].GetVariantVectorPtr();
	Urho3D::VariantMap* shapeop_vertex_var_map = (*vertices)[i].GetVariantMapPtr();
	return (*shapeop_vertex_var_map)["raw_index"].GetInt();
}

void SetConstraintProcessedIndex(
	Urho3D::Variant& constraint,
	int i, // index into constraint's "vertices" VariantVector of ShapeOpVertex objects
	int processed_index // index into master processed vertex list to store
)
{
	Urho3D::VariantMap* var_map = constraint.GetVariantMapPtr();

	Urho3D::VariantVector* vertices = (*var_map)["vertices"].GetVariantVectorPtr();
	Urho3D::VariantMap* shapeop_vertex_var_map = (*vertices)[i].GetVariantMapPtr();
	(*shapeop_vertex_var_map)["processed_index"] = Urho3D::Variant(processed_index);
}

int GetConstraintProcessedIndex(
	Urho3D::Variant& constraint,
	int i // index into constraint's "vertices" VariantVector of ShapeOpVertex objects
)
{
	Urho3D::VariantMap* var_map = constraint.GetVariantMapPtr();

	Urho3D::VariantVector* vertices = (*var_map)["vertices"].GetVariantVectorPtr();
	Urho3D::VariantMap* shapeop_vertex_var_map = (*vertices)[i].GetVariantMapPtr();
	return (*shapeop_vertex_var_map)["processed_index"].GetInt();
}

//
int GetConstraintIds(
	Urho3D::Variant& constraint,
	std::vector<int>& ids
)
{
	if (!ShapeOpConstraint_Verify(constraint)) {
		std::cout << "GetConstraintIds --- constraint failed to verify" << std::endl;
		return 0;
	}

	Urho3D::VariantMap* var_map = constraint.GetVariantMapPtr();
	Urho3D::VariantVector* vertices = (*var_map)["vertices"].GetVariantVectorPtr();
	for (unsigned i = 0; i < vertices->Size(); ++i) {
		Urho3D::VariantMap* shapeop_vertex_var_map = (*vertices)[i].GetVariantMapPtr();
		int id = (*shapeop_vertex_var_map)["processed_index"].GetInt();
		if (id < 0) {
			std::cout << "GetConstraintIds --- constraint processed_index not yet set" << std::endl;
			return 0;
		}
		ids.push_back(id);
	}

	return 1;
}

void ShapeOpConstraint_Print(
	Urho3D::Variant& constraint
)
{
	if (!ShapeOpConstraint_Verify(constraint)) {
		std::cout << "ShapeOpConstraint_Print --- constraint failed to verify" << std::endl;
		return;
	}

	Urho3D::VariantMap* var_map = constraint.GetVariantMapPtr();
	Urho3D::String constraintType = (*var_map)["constraintType"].GetString();
	float weight = (*var_map)["weight"].GetFloat();
	std::cout << constraintType.CString() << " weight=" << weight << std::endl;
	Urho3D::VariantVector* shapeop_vertices = (*var_map)["vertices"].GetVariantVectorPtr();
	for (int i = 0; i < (int)shapeop_vertices->Size(); ++i) {
		Urho3D::VariantMap* var_map2 = (*shapeop_vertices)[i].GetVariantMapPtr();
		Urho3D::Vector3 v = (*var_map2)["coords"].GetVector3();
		int raw_index = (*var_map2)["raw_index"].GetInt();
		int processed_index = (*var_map2)["processed_index"].GetInt();
		std::cout << "\t(" << v.x_ << "," << v.y_ << "," << v.z_ << ") raw=" << raw_index << " processed=" << processed_index << std::endl;
	}
}