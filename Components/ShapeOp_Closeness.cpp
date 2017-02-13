#include "ShapeOp_Closeness.h"

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

String ShapeOp_Closeness::iconTexture = "Textures/Icons/DefaultIcon.png";

ShapeOp_Closeness::ShapeOp_Closeness(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("ShapeOpCloseness");
	SetFullName("ShapeOp Closeness");
	SetDescription("...");

	inputSlots_[0]->SetName("Position");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Target position");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Weight");
	inputSlots_[1]->SetVariableName("W");
	inputSlots_[1]->SetDescription("Weight of edge constraint");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(100.0f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("EdgeStrain");
	outputSlots_[0]->SetVariableName("ES");
	outputSlots_[0]->SetDescription("EdgeStrain ShapeOp constraint");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void ShapeOp_Closeness::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	if (
		inSolveInstance[0].GetType() != VAR_VECTOR3
		)
	{
		SetAllOutputsNull(outSolveInstance);
		URHO3D_LOGWARNING("ShapeOp_Closeness --- invalid input");
		return;
	}

	Vector3 start_coords = inSolveInstance[0].GetVector3();
	float weight = inSolveInstance[1].GetFloat();
	if (weight <= 0.0f) {
		SetAllOutputsNull(outSolveInstance);
		URHO3D_LOGWARNING("ShapeOp_Closeness --- weight must be > 0.0f");
		return;
	}

	Variant start = ShapeOpVertex_Make(start_coords);
	VariantVector shapeop_vertices;
	shapeop_vertices.Push(start);

	VariantMap var_map;
	var_map["type"] = Variant("ShapeOpConstraint");
	var_map["constraintType"] = Variant("Closeness");
	var_map["weight"] = weight;
	var_map["vertices"] = shapeop_vertices;

	Variant constraint = Variant(var_map);
	outSolveInstance[0] = constraint;
}