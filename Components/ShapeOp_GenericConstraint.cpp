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

	if (verts.At(0).GetType() != VAR_VECTOR3)
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