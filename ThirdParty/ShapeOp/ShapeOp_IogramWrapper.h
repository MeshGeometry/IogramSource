#pragma once

#include <vector>

#include <Urho3D/Container/Str.h>
#include <Urho3D/Core/Variant.h>

////////////////////
// ShapeOpConstraint

bool IsValidShapeOpConstraint(
	const Urho3D::String& constraint_type,
	const Urho3D::VariantVector& id_list,
	int nb_points,
	float weight
);

Urho3D::Variant ShapeOpConstraint_Make(
	const Urho3D::String& constraint_type,
	const Urho3D::VariantVector& id_list,
	int nb_points,
	float weight
);

bool ShapeOpConstraint_Verify(const Urho3D::Variant& constraint);

Urho3D::String ShapeOpConstraint_constraintType(const Urho3D::Variant& constraint);
std::vector<int> ShapeOpConstraint_ids(const Urho3D::Variant& constraint);
int ShapeOpConstraint_nb_ids(const Urho3D::Variant& constraint);
double ShapeOpConstraint_weight(const Urho3D::Variant& constraint);

std::vector<double> ShapeOp_TriMeshToPoints(const Urho3D::Variant& tri_mesh);

Urho3D::VariantVector ShapeOp_PointsToVertexList(const std::vector<double>& pts);

//////////////
// VertexForce

Urho3D::Variant ShapeOpVertexForce_Make(
	const Urho3D::Vector3& force_on_vertex,
	int vertex_id
);

int ShapeOpVertexForce_id(const Urho3D::Variant& vertex_force);
std::vector<double> ShapeOpVertexForce_force(const Urho3D::Variant& vertex_force);