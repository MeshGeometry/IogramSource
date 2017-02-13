#include "ShapeOp_Solve.h"

#include <assert.h>

#include <iostream>
#include <vector>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ShapeOp_API.h"
#include "ShapeOp_IogramWrapper.h"

#include "TriMesh.h"

using namespace Urho3D;

namespace {

void WeldVertices(
	const Vector<Vector3>& vertices,
	Vector<Vector3>& welded_vertices,
	Vector<int>& new_indices, // new_indices.Size() == vertices.Size() and
							  // new_indices[i] = j means that vertices[i]
							  // is the same as welded_vertices[j]
	float eps
)
{
	float epsSquared = eps * eps;

	if (vertices.Size() <= 1) {
		URHO3D_LOGWARNING("WeldVertices-- - vertices.Size() <= 1, nothing to do");
		return;
	}

	std::vector<int> vertexDuplicates_(vertices.Size());

	for (int i = 0; i < (int)vertices.Size(); ++i) {
		vertexDuplicates_[i] = i; // Assume not a duplicate
		for (int j = 0; j < i; ++j) {
			if ((vertices[j] - vertices[i]).LengthSquared() < epsSquared) {
				vertexDuplicates_[i] = j;
				break;
			}
		}
	}

	std::vector<int> vertexUniques_ = vertexDuplicates_;
	std::sort(vertexUniques_.begin(), vertexUniques_.end());
	vertexUniques_.erase(std::unique(vertexUniques_.begin(), vertexUniques_.end()), vertexUniques_.end());

	for (int i = 0; i < vertexUniques_.size(); ++i) {
		int k = vertexUniques_[i];
		welded_vertices.Push(vertices[k]);
	}

	for (int j = 0; j < (int)vertices.Size(); ++j) {
		int orig_j = vertexDuplicates_[j];
		int uniq_j = find(vertexUniques_.begin(), vertexUniques_.end(), orig_j) - vertexUniques_.begin();
		new_indices.Push(uniq_j);
	}

	assert(new_indices.Size() == vertices.Size());
}

} // namespace

String ShapeOp_Solve::iconTexture = "Textures/Icons/DefaultIcon.png";

ShapeOp_Solve::ShapeOp_Solve(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("ShapeOpSolve");
	SetFullName("ShapeOp Solve");
	SetDescription("...");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Constraint List");
	inputSlots_[0]->SetVariableName("CL");
	inputSlots_[0]->SetDescription("Constraint List");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);

	inputSlots_[1]->SetName("WeldDist");
	inputSlots_[1]->SetVariableName("WD");
	inputSlots_[1]->SetDescription("Weld points within distance");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(0.0001f));
	inputSlots_[1]->DefaultSet();

	// unfinished
	outputSlots_[0]->SetName("Points");
	outputSlots_[0]->SetVariableName("Pts");
	outputSlots_[0]->SetDescription("Point list output");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void ShapeOp_Solve::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	//
	// TMP: turned off due to num input slot confusion
	/*
	float weld_eps = inSolveInstance[1].GetFloat();
	if (weld_eps <= 0.0f) {
		std::cout << "weld_eps = " << weld_eps << std::endl;
		URHO3D_LOGWARNING("ShapeOp_Solve --- WeldDist must be > 0.0f");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	*/

	//////////////////////////////////////////////////////
	// Go through the input and find the valid constraints

	VariantVector uncleaned_input = inSolveInstance[0].GetVariantVector();
	VariantVector constraints;
	for (unsigned i = 0; i < uncleaned_input.Size(); ++i) {

		Variant var = uncleaned_input[i];
		if (ShapeOpConstraint_Verify(var)) {
			constraints.Push(var);
		}
	}
	if (constraints.Size() <= 0) {
		SetAllOutputsNull(outSolveInstance);
		URHO3D_LOGWARNING("ShapeOp_Solve --- no valid constraints found at ConstraintList");
		return;
	}
	URHO3D_LOGINFO("ShapeOp_Solve --- found valid constraints, beginning vertex processing....");

	///////////////////////////////////////////////////////////////////
	// Collect points into master point list, then weld nearby vertices
	// letting the constraints know about all indices, into both
	// master list and welded list

	int raw_index = 0;
	Vector<Vector3> raw_vertices;
	for (unsigned i = 0; i < constraints.Size(); ++i) {

		VariantMap* var_map = constraints[i].GetVariantMapPtr();


		VariantVector* shapeop_vertices = (*var_map)["vertices"].GetVariantVectorPtr();

		int num_shapeop_vertices = shapeop_vertices->Size();

		for (unsigned j = 0; j < shapeop_vertices->Size(); ++j) {
			VariantMap* var_map2 = (*shapeop_vertices)[j].GetVariantMapPtr();
			Vector3 v = (*var_map2)["coords"].GetVector3();
			std::cout << "vertex: " << v.x_ << " " << v.y_ << " " << v.z_ << std::endl;
			raw_vertices.Push(v);

			SetConstraintRawIndex(constraints[i], j, raw_index);

			//
			++raw_index;
		}
	}
	assert(raw_index == (int)raw_vertices.Size());

	Vector<Vector3> welded_vertices;
	Vector<int> new_indices;
	WeldVertices(raw_vertices, welded_vertices, new_indices, 0.001f);

	// TMP: debug output
	std::cout << "raw_vertices:" << std::endl;
	for (int i = 0; i < std::min(10, (int)raw_vertices.Size()); ++i) {
		Vector3 v = raw_vertices[i];
		std::cout << "i: " << v.x_ << " " << v.y_ << " " << v.z_ << std::endl;
	}
	std::cout << "welded_vertices:" << std::endl;
	for (int i = 0; i < std::min(10, (int)welded_vertices.Size()); ++i) {
		Vector3 v = welded_vertices[i];
		std::cout << "i: " << v.x_ << " " << v.y_ << " " << v.z_ << std::endl;
	}
	std::cout << "new_indices:" << std::endl;
	for (int i = 0; i < std::min(10, (int)new_indices.Size()); ++i) {
		std::cout << "i: " << new_indices[i] << std::endl;
	}

	// now that:
	//   welded_vertices
	//   new_indices
	// are prepared, go through the constraints and set the corresponding welded (processed) indices
	for (unsigned i = 0; i < constraints.Size(); ++i) {

		VariantMap* var_map = constraints[i].GetVariantMapPtr();


		VariantVector* shapeop_vertices = (*var_map)["vertices"].GetVariantVectorPtr();

		int num_shapeop_vertices = shapeop_vertices->Size();

		for (unsigned j = 0; j < shapeop_vertices->Size(); ++j) {
			VariantMap* var_map2 = (*shapeop_vertices)[j].GetVariantMapPtr();

			int current_raw_index = (*var_map2)["raw_index"].GetInt();
			if (current_raw_index == -1) {
				SetAllOutputsNull(outSolveInstance);
				URHO3D_LOGWARNING("ShapeOp_Solve --- current_raw_index == -1, something went wrong");
				return;
			}
			int processed_index = new_indices[current_raw_index];
			SetConstraintProcessedIndex(constraints[i], j, processed_index);
		}
	}

	// TMP: debug output
	for (unsigned i = 0; i < constraints.Size(); ++i) {
		ShapeOpConstraint_Print(constraints[i]);
	}

	///////////////////////////////
	// ShapeOp API calls start here

	////////////////////////////////////////////
	// 1) Create the solver with #shapeop_create

	ShapeOpSolver* op = shapeop_create();

	//////////////////////////////////////////////
	// 2) Set the vertices with #shapeop_setPoints

	std::vector<double> pts_in;
	for (int i = 0; i < (int)welded_vertices.Size(); ++i) {
		Vector3 v = welded_vertices[i];
		pts_in.push_back((double)v.x_);
		pts_in.push_back((double)v.y_);
		pts_in.push_back((double)v.z_);
	}

	int nb_points = (int)(pts_in.size() / 3);
	shapeop_setPoints(op, pts_in.data(), nb_points);

	////////////////////////////////////////////////////////////////////////////////////
	// 3A) Setup the constraints with #shapeop_addConstraint and #shapeop_editConstraint

	for (unsigned i = 0; i < constraints.Size(); ++i) {

		Variant constraint = constraints[i];


	}

	// all ids are captured here so their data won't go out of scope after added to the solver

	// change how the constraints are added ....

	/*
	std::vector<std::vector<int> > all_ids;
	for (unsigned i = 0; i < constraint_list.Size(); ++i) {
		std::vector<int> ids = ShapeOpConstraint_ids(constraint_list[i]);
		all_ids.push_back(ids);
	}

	for (unsigned i = 0; i < constraint_list.Size(); ++i) {
		Variant constraint = constraint_list[i];

		shapeop_addConstraint(
			op,
			ShapeOpConstraint_constraintType(constraint).CString(),
			all_ids[i].data(),
			ShapeOpConstraint_nb_ids(constraint),
			ShapeOpConstraint_weight(constraint)
		);
	}

	URHO3D_LOGINFO("ShapeOp_Solve --- Constraints added");
	*/

	////////////////////////////////////////////////////
	// 3B) Setup the forces with #shapeop_addVertexForce

	// ... not sure if we're still adding forces to individual vertices

	URHO3D_LOGINFO("ShapeOp_Solve --- Vertex forces added");

	//////////////////////////////////////////////////////////////////////
	// 4) Initialize the solver with #shapeop_init or #shapeop_initDynamic

	//shapeop_init(op);
	//shapeop_initDynamic(op, 2.0, 0.1, 0.1);

	//////////////////////////////////
	// 5) Optimize with #shapeop_solve

	//shapeop_solve(op, 100);

	///////////////////////////////////////////////////
	// 6) Get back the vertices with #shapeop_getPoints

	// Block below has debug stuff, can be simplified after
	double* pts_out = new double[3 * nb_points];
	shapeop_getPoints(op, pts_out, nb_points);
	std::vector<double> pts_out_vec;
	for (int i = 0; i < 3 * nb_points; ++i) {
		std::cout << pts_out[i] << " ";
		pts_out_vec.push_back(pts_out[i]);
	}
	delete pts_out;

	////////////////////////////////////////////
	// 7) Delete the solver with #shapeop_delete

	shapeop_delete(op);
}