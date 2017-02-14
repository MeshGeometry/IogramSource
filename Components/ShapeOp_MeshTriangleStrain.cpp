#include "ShapeOp_MeshTriangleStrain.h"

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

String ShapeOp_MeshTriangleStrain::iconTexture = "Textures/Icons/DefaultIcon.png";

ShapeOp_MeshTriangleStrain::ShapeOp_MeshTriangleStrain(Context* context) : IoComponentBase(context, 0, 0)
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

void ShapeOp_MeshTriangleStrain::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{

	VariantVector verts;
	VariantVector faces;

	if (TriMesh_Verify(inSolveInstance[0]))
	{
		verts = TriMesh_GetVertexList(inSolveInstance[0]);
		faces = TriMesh_GetFaceList(inSolveInstance[0]);
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

	for (int i = 0; i < faces.Size()/3; i++)
	{

		Vector3 vA = verts[faces[3 * i + 0].GetInt()].GetVector3();
		Vector3 vB = verts[faces[3 * i + 1].GetInt()].GetVector3();
		Vector3 vC = verts[faces[3 * i + 2].GetInt()].GetVector3();

		VariantVector shapeop_vertices = {
			ShapeOpVertex_Make(vA),
			ShapeOpVertex_Make(vB),
			ShapeOpVertex_Make(vC)
		};

		VariantMap var_map;
		var_map["type"] = Variant("ShapeOpConstraint");
		var_map["constraintType"] = Variant("TriangleStrain");
		var_map["weight"] = weight;
		var_map["vertices"] = shapeop_vertices;

		strainsOut.Push(var_map);

	}


	outSolveInstance[0] = strainsOut;
}