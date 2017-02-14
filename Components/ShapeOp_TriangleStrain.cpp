#include "ShapeOp_TriangleStrain.h"

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

String ShapeOp_TriangleStrain::iconTexture = "Textures/Icons/DefaultIcon.png";

ShapeOp_TriangleStrain::ShapeOp_TriangleStrain(Context* context) : IoComponentBase(context, 6, 1)
{
	SetName("ShapeOpTriangleStrain");
	SetFullName("ShapeOp Triangle Strain");
	SetDescription("...");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("vertexA");
	inputSlots_[0]->SetVariableName("vA");
	inputSlots_[0]->SetDescription("Triangle vertex A");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("vertexB");
	inputSlots_[1]->SetVariableName("vB");
	inputSlots_[1]->SetDescription("Triangle vertex B");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[2]->SetName("vertexC");
	inputSlots_[2]->SetVariableName("vC");
	inputSlots_[2]->SetDescription("Triangle vertex C");
	inputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[3]->SetName("Weight");
	inputSlots_[3]->SetVariableName("W");
	inputSlots_[3]->SetDescription("Weight of edge constraint");
	inputSlots_[3]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(Variant(1.0f));
	inputSlots_[3]->DefaultSet();

	inputSlots_[4]->SetName("rangeMin");
	inputSlots_[4]->SetVariableName("rMin");
	inputSlots_[4]->SetDescription("rangeMin for ShapeOp EdgeConstraint");
	inputSlots_[4]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[4]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[5]->SetName("rangeMax");
	inputSlots_[5]->SetVariableName("rMax");
	inputSlots_[5]->SetDescription("rangeMax for ShapeOp EdgeConstraint");
	inputSlots_[5]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[5]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("TriangleStrain");
	outputSlots_[0]->SetVariableName("ES");
	outputSlots_[0]->SetDescription("TriangleStrain ShapeOp constraint");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void ShapeOp_TriangleStrain::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	if (
		inSolveInstance[0].GetType() != VAR_VECTOR3 ||
		inSolveInstance[1].GetType() != VAR_VECTOR3 ||
		inSolveInstance[2].GetType() != VAR_VECTOR3 ||
		!(inSolveInstance[3].GetType() == VAR_FLOAT || inSolveInstance[3].GetType() == VAR_DOUBLE)
		)
	{
		SetAllOutputsNull(outSolveInstance);
		URHO3D_LOGWARNING("ShapeOp_EdgeStrain --- invalid input");
		return;
	}
	Vector3 a_coords = inSolveInstance[0].GetVector3();
	Vector3 b_coords = inSolveInstance[1].GetVector3();
	Vector3 c_coords = inSolveInstance[2].GetVector3();
	float weight = inSolveInstance[3].GetFloat();
	if (weight <= 0.0f) {
		SetAllOutputsNull(outSolveInstance);
		URHO3D_LOGWARNING("ShapeOp_TriangleStrain --- weight must be > 0.0");
		return;
	}

	Variant a = ShapeOpVertex_Make(a_coords);
	Variant b = ShapeOpVertex_Make(b_coords);
	Variant c = ShapeOpVertex_Make(c_coords);
	VariantVector shapeop_vertices;
	shapeop_vertices.Push(a);
	shapeop_vertices.Push(b);
	shapeop_vertices.Push(c);

	VariantMap var_map;
	var_map["type"] = Variant("ShapeOpConstraint");
	var_map["constraintType"] = Variant("TriangleStrain");
	var_map["weight"] = weight;
	var_map["vertices"] = shapeop_vertices;
	var_map["constraint_id"] = -1;

	// User has the option of supplying minRange and maxRange values if defaults won't do
	VariantType type_minRange = inSolveInstance[4].GetType();
	VariantType type_maxRange = inSolveInstance[5].GetType();
	if (
		(type_minRange == VAR_INT || type_minRange == VAR_FLOAT || type_minRange == VAR_DOUBLE) &&
		(type_maxRange == VAR_INT || type_maxRange == VAR_FLOAT || type_maxRange == VAR_DOUBLE)
		)
	{
		double minRange = inSolveInstance[4].GetDouble();
		double maxRange = inSolveInstance[5].GetDouble();
		if (
			minRange >= 0.0 &&
			maxRange >= minRange
			)
		{
			var_map["minRange"] = minRange;
			var_map["maxRange"] = maxRange;
			var_map["editFlag"] = 1;
		}
	}

	Variant constraint = Variant(var_map);
	outSolveInstance[0] = constraint;
}