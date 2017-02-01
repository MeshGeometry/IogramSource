#include "ShapeOp_OrientationConstraint.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include <ShapeOp/api/API.h>

#include "TriMesh.h"

using namespace Urho3D;

String ShapeOp_OrientationConstraint::iconTexture = "Textures/Icons/DefaultIcon.png";

ShapeOp_OrientationConstraint::ShapeOp_OrientationConstraint(Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("OrientationConstraint");
	SetFullName("ShapeOp Orientation Constraint");
	SetDescription("...");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh input");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh input");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("Mesh output");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Mesh output");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void ShapeOp_OrientationConstraint::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	if (!IsAllInputValid(inSolveInstance)) {
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	Variant mesh_in = inSolveInstance[0];
	if (!TriMesh_Verify(mesh_in)) {
		URHO3D_LOGWARNING("Mesh input must be a TriMesh");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	VariantVector vertex_list = TriMesh_GetVertexList(mesh_in);
	VariantVector face_list = TriMesh_GetFaceList(mesh_in);

	/* SHAPEOP */

	// 1) Create the solver with #shapeop_create

	//ShapeOpSolver* op = shapeop_create();

	// 7) Delete the solver with #shapeop_delete

	//shapeop_delete(op);

	outSolveInstance[0] = TriMesh_Make(vertex_list, face_list);
}