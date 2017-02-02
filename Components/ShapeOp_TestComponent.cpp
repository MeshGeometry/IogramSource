#include "ShapeOp_TestComponent.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

//#include <ShapeOp/api/API.h>
//#include <ShapeOp/src/Solver.h>

//#include "Solver.h"
#include "API.h"

#include "TriMesh.h"

using namespace Urho3D;

String ShapeOp_TestComponent::iconTexture = "Textures/Icons/DefaultIcon.png";

ShapeOp_TestComponent::ShapeOp_TestComponent(Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("ShapeOpTest");
	SetFullName("ShapeOp Test Component");
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

void ShapeOp_TestComponent::SolveInstance(
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
	ShapeOpSolver* op = shapeop_create();

	// 2) Set the vertices with #shapeop_setPoints
	std::vector<double> points_in;
	for (int i = 0; i < vertex_list.Size(); ++i) {
		Vector3 v = vertex_list[i].GetVector3();
		points_in.push_back(v.x_);
		points_in.push_back(v.y_);
		points_in.push_back(v.z_);
	}
	int nb_points = (int)(points_in.size() / 3);
	shapeop_setPoints(op, points_in.data(), nb_points);

	// 3) Setup the constraints and forces with #shapeop_addConstraint and #shapeop_editConstraint
	int ids[] = { 0, 1 };
	//shapeop_addConstraint(op, "EdgeStrain", ids, 2, 1.0);
	//double gravity[3] = { 0.0, -0.1, 0.0 };
	//shapeop_addGravityForce(op, gravity);

	// 4) Initialize the solver with #shapeop_init or #shapeop_initDynamic
	//shapeop_init(op);

	// 5) Optimize with #shapeop_solve
	//shapeop_solve(op, 4);

	// 6) Get back the vertices with #shapeop_getPoints
	//double* points_out = new double[3 * nb_points];
	std::vector<double> points_out(3 * nb_points);
	shapeop_getPoints(op, points_out.data(), nb_points);


	// 7) Delete the solver with #shapeop_delete
	shapeop_delete(op);

	VariantVector new_vertex_list;
	for (int i = 0; i < points_out.size(); i += 3) {
		Vector3 v(points_out[i], points_out[i + 1], points_out[i + 2]);
		new_vertex_list.Push(v);
	}

	outSolveInstance[0] = TriMesh_Make(new_vertex_list, face_list);
}