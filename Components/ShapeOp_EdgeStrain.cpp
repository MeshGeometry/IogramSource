#include "ShapeOp_EdgeStrain.h"

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

String ShapeOp_EdgeStrain::iconTexture = "Textures/Icons/DefaultIcon.png";

ShapeOp_EdgeStrain::ShapeOp_EdgeStrain(Context* context) : IoComponentBase(context, 5, 1)
{
	SetName("ShapeOpEdgeStrain");
	SetFullName("ShapeOp Edge Strain");
	SetDescription("...");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Start");
	inputSlots_[0]->SetVariableName("S");
	inputSlots_[0]->SetDescription("Start point of edge");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("End");
	inputSlots_[1]->SetVariableName("E");
	inputSlots_[1]->SetDescription("End point of edge");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[2]->SetName("Weight");
	inputSlots_[2]->SetVariableName("W");
	inputSlots_[2]->SetDescription("Weight of edge constraint");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(Variant(1.0f));
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("rangeMin");
	inputSlots_[3]->SetVariableName("rMin");
	inputSlots_[3]->SetDescription("rangeMin for ShapeOp EdgeConstraint");
	inputSlots_[3]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[4]->SetName("rangeMax");
	inputSlots_[4]->SetVariableName("rMax");
	inputSlots_[4]->SetDescription("rangeMax for ShapeOp EdgeConstraint");
	inputSlots_[4]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[4]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("EdgeStrain");
	outputSlots_[0]->SetVariableName("ES");
	outputSlots_[0]->SetDescription("EdgeStrain ShapeOp constraint");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void ShapeOp_EdgeStrain::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	if (
		inSolveInstance[0].GetType() != VAR_VECTOR3 ||
		inSolveInstance[1].GetType() != VAR_VECTOR3 ||
		!(inSolveInstance[2].GetType() == VAR_FLOAT || inSolveInstance[2].GetType() == VAR_DOUBLE)
		)
	{
		SetAllOutputsNull(outSolveInstance);
		URHO3D_LOGWARNING("ShapeOp_EdgeStrain --- invalid input");
		return;
	}
	Vector3 start_coords = inSolveInstance[0].GetVector3();
	Vector3 end_coords = inSolveInstance[1].GetVector3();
	float weight = inSolveInstance[2].GetFloat();
	if (weight <= 0.0f) {
		SetAllOutputsNull(outSolveInstance);
		URHO3D_LOGWARNING("ShapeOp_EdgeStrain --- weight must be > 0.0f");
		return;
	}

	Variant start = ShapeOpVertex_Make(start_coords);
	Variant end = ShapeOpVertex_Make(end_coords);
	VariantVector shapeop_vertices;
	shapeop_vertices.Push(start);
	shapeop_vertices.Push(end);

	VariantMap var_map;
	var_map["type"] = Variant("ShapeOpConstraint");
	var_map["constraintType"] = Variant("EdgeStrain");
	var_map["weight"] = weight;
	var_map["vertices"] = shapeop_vertices;

	// User has the option of supplying minRange and maxRange values if defaults won't do
	VariantType type_minRange = inSolveInstance[3].GetType();
	VariantType type_maxRange = inSolveInstance[4].GetType();
	if (
		(type_minRange == VAR_INT || type_minRange == VAR_FLOAT || type_minRange == VAR_DOUBLE) &&
		(type_maxRange == VAR_INT || type_maxRange == VAR_FLOAT || type_maxRange == VAR_DOUBLE)
		)
	{
		double minRange = inSolveInstance[3].GetDouble();
		double maxRange = inSolveInstance[4].GetDouble();
		if (
			minRange >= 0.0 &&
			maxRange >= minRange
			)
		{
			double length = (start_coords - end_coords).Length();

			var_map["length"] = length;
			var_map["minRange"] = minRange;
			var_map["maxRange"] = maxRange;
			var_map["editFlag"] = 1;
		}
	}

	Variant constraint = Variant(var_map);
	outSolveInstance[0] = constraint;
}