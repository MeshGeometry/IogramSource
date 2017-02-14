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