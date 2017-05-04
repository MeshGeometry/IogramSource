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

	outputSlots_[0]->SetName("Closeness");
	outputSlots_[0]->SetVariableName("CS");
	outputSlots_[0]->SetDescription("Closeness ShapeOp constraint");
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