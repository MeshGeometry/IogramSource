#pragma once

#include <vector>

#include <Urho3D/Container/Str.h>
#include <Urho3D/Core/Variant.h>

////////////////////
// ShapeOpConstraint

bool ShapeOpConstraint_Verify(const Urho3D::Variant& constraint);

Urho3D::String ShapeOpConstraint_constraintType(const Urho3D::Variant& constraint);

double ShapeOpConstraint_weight(const Urho3D::Variant& constraint);

////////////////
// ShapeOpVertex

Urho3D::Variant ShapeOpVertex_Make(
	const Urho3D::Vector3& v
);

void SetConstraintRawIndex(
	Urho3D::Variant& constraint,
	int i, // index into constraint's "vertices" VariantVector of ShapeOpVertex objects
	int raw_index // index into master raw vertex list to store
);

void SetConstraintProcessedIndex(
	Urho3D::Variant& constraint,
	int i, // index into constraint's "vertices" VariantVector of ShapeOpVertex objects
	int processed_index // index into master processed vertex list to store
);

int GetConstraintProcessedIndex(
	Urho3D::Variant& constraint,
	int i // index into constraint's "vertices" VariantVector of ShapeOpVertex objects
);

int GetConstraintIds(
	Urho3D::Variant& constraint,
	std::vector<int>& ids
);

void ShapeOpConstraint_Print(
	Urho3D::Variant& constraint
);

int ShapeOpConstraint_NeedsEdit(
	Urho3D::Variant& constraint
);

void ShapeOpConstraint_SetConstraintId(
	Urho3D::Variant& constraint,
	int constraint_id
);