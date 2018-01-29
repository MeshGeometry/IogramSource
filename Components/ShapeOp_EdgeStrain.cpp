//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


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

ShapeOp_EdgeStrain::ShapeOp_EdgeStrain(Context* context) : IoComponentBase(context, 6, 1)
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

	inputSlots_[2]->SetName("Length");
	inputSlots_[2]->SetVariableName("L");
	inputSlots_[2]->SetDescription("Target length");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
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

	Vector3 start_coords = inSolveInstance[0].GetVector3();
	Vector3 end_coords = inSolveInstance[1].GetVector3();
	float targetLength = Abs(inSolveInstance[2].GetFloat());
	float weight = Abs(inSolveInstance[3].GetFloat());

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
			double length = (start_coords - end_coords).Length();

			var_map["length"] = targetLength;
			var_map["minRange"] = minRange;
			var_map["maxRange"] = maxRange;
			var_map["editFlag"] = 1;
		}
	}

	Variant constraint = Variant(var_map);
	outSolveInstance[0] = constraint;
}