#pragma once

#include <Urho3D/Container/Str.h>
#include <Urho3D/Core/Variant.h>

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