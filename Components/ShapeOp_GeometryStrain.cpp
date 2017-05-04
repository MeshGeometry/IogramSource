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


#include "ShapeOp_GeometryStrain.h"

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

String ShapeOp_GeometryStrain::iconTexture = "Textures/Icons/DefaultIcon.png";

ShapeOp_GeometryStrain::ShapeOp_GeometryStrain(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("GeometryStrain");
	SetFullName("Geometry Strain");
	SetDescription("...");

	AddInputSlot(
		"Geometry",
		"G",
		"Mesh or polyline geometry",
		VAR_VARIANTMAP,
		DataAccess::ITEM
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
		"EdgeStrain",
		"ES",
		"Edge Strain",
		VAR_VARIANTMAP,
		DataAccess::LIST
		);

}

void ShapeOp_GeometryStrain::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	
	VariantVector verts;
	Vector<Pair<int, int>> edges;
	
	if(TriMesh_Verify(inSolveInstance[0]))
	{
		verts = TriMesh_GetVertexList(inSolveInstance[0]);
		edges = TriMesh_ComputeEdges(inSolveInstance[0]);
	}
	else if (Polyline_Verify(inSolveInstance[0]))
	{
		verts = Polyline_GetVertexList(inSolveInstance[0]);
		edges = Polyline_ComputeEdges(inSolveInstance[0]);
	}
	else
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//create the constraints
	VariantVector strainsOut;

	//get general params
	float weight = inSolveInstance[1].GetFloat();
	float min = inSolveInstance[2].GetFloat();
	float max = inSolveInstance[3].GetFloat();

	for (int i = 0; i < edges.Size(); i++)
	{
		
		Vector3 vA = verts[edges[i].first_].GetVector3();
		Vector3 vB = verts[edges[i].second_].GetVector3();
		
		Variant start = ShapeOpVertex_Make(vA);
		Variant end = ShapeOpVertex_Make(vB);
		VariantVector shapeop_vertices;
		shapeop_vertices.Push(start);
		shapeop_vertices.Push(end);

		VariantMap var_map;
		var_map["type"] = Variant("ShapeOpConstraint");
		var_map["constraintType"] = Variant("EdgeStrain");
		var_map["weight"] = weight;
		var_map["vertices"] = shapeop_vertices;

		strainsOut.Push(var_map);

	}


	outSolveInstance[0] = strainsOut;
}