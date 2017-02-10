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

ShapeOp_EdgeStrain::ShapeOp_EdgeStrain(Context* context) : IoComponentBase(context, 3, 1)
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

	Variant constraint = Variant(var_map);
	outSolveInstance[0] = constraint;
}