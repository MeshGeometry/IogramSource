#include "ShapeOp_GenericConstraint.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ShapeOp_IogramWrapper.h"

#include "TriMesh.h"
#include "Polyline.h"

using namespace Urho3D;

namespace {
} // namespace

String ShapeOp_GenericConstraint::iconTexture = "Textures/Icons/DefaultIcon.png";

ShapeOp_GenericConstraint::ShapeOp_GenericConstraint(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("GeometryStrain");
	SetFullName("Geometry Strain");
	SetDescription("...");

	AddInputSlot(
		"Type",
		"CT",
		"Constraint type.",
		VAR_STRING,
		DataAccess::ITEM
		);

	AddInputSlot(
		"Vertices",
		"V",
		"Vertices that define the constraint",
		VAR_VECTOR3,
		DataAccess::LIST
		);


	AddInputSlot(
		"Weight",
		"W",
		"Weight of strain",
		VAR_FLOAT,
		DataAccess::ITEM,
		1.0f
		);

	AddInputSlot(
		"Max",
		"MX",
		"Max of strain",
		VAR_FLOAT,
		DataAccess::ITEM,
		100.0f
		);

	AddInputSlot(
		"Min",
		"MN",
		"Min strain.",
		VAR_FLOAT,
		DataAccess::ITEM,
		0.00f
		);

	AddOutputSlot(
		"Constraint",
		"C",
		"Generic Constraint",
		VAR_VARIANTMAP,
		DataAccess::ITEM
		);

}

void ShapeOp_GenericConstraint::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{

	//get general params
	String cType = inSolveInstance[0].GetString();
	VariantVector verts = inSolveInstance[1].GetVariantVector();
	float weight = inSolveInstance[2].GetFloat();
	float min = inSolveInstance[3].GetFloat();
	float max = inSolveInstance[4].GetFloat();

	if (verts.Empty())
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//get the verts
	VariantVector shapeop_vertices;
	for (int i = 0; i < verts.Size(); i++)
	{
		if (verts[i].GetType() == VAR_VECTOR3)
		{
			Variant v = ShapeOpVertex_Make(verts[i].GetVector3());
			shapeop_vertices.Push(v);
		}

	}

	//create the constraint
	VariantMap var_map;
	var_map["type"] = Variant("ShapeOpConstraint");
	var_map["constraintType"] = cType;
	var_map["weight"] = weight;
	var_map["vertices"] = shapeop_vertices;

	outSolveInstance[0] = var_map;
}