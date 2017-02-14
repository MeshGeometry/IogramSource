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

////////////////////////////////////////////////////
// Helper functions collected in anonymous namespace
namespace {

// Input slot data extraction for solver parameters
void GetInitializationParameters(
	const Variant& mass_var,
	double& mass,
	const Variant& damping_var,
	double& damping,
	const Variant& timestep_var,
	double& timestep,
	const Variant& iterations_var,
	int& iterations
)
{
	if (mass_var.GetType() == VAR_FLOAT || mass_var.GetType() == VAR_DOUBLE) {
		double mass_ = mass_var.GetDouble();
		if (mass_ > 0.0) {
			mass = mass_;
		}
	}
	if (damping_var.GetType() == VAR_FLOAT || damping_var.GetType() == VAR_DOUBLE) {
		double damping_ = damping_var.GetDouble();
		if (damping_ > 0.0) {
			damping = damping_;
		}
	}
	if (timestep_var.GetType() == VAR_FLOAT || timestep_var.GetType() == VAR_DOUBLE) {
		double timestep_ = timestep_var.GetDouble();
		if (timestep_ > 0.0) {
			timestep = timestep_;
		}
	}
	if (iterations_var.GetType() == VAR_INT) {
		int iterations_ = iterations_var.GetInt();
		if (iterations_ > 0) {
			iterations = iterations_;
		}
	}
}

void SetUpRawVertices(
	VariantVector& constraints,
	Vector<Vector3>& raw_vertices
)
{
	int raw_index = 0;
	for (unsigned i = 0; i < constraints.Size(); ++i) {

		VariantMap* var_map = constraints[i].GetVariantMapPtr();
		VariantVector* shapeop_vertices = (*var_map)["vertices"].GetVariantVectorPtr();

		for (unsigned j = 0; j < shapeop_vertices->Size(); ++j) {
			VariantMap* var_map2 = (*shapeop_vertices)[j].GetVariantMapPtr();
			Vector3 v = (*var_map2)["coords"].GetVector3();
			raw_vertices.Push(v);
			SetConstraintRawIndex(constraints[i], j, raw_index);
			++raw_index;
		}
	}
}

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

void UpdateConstraintsAfterWelding(
	VariantVector& constraints,
	const Vector<int>& new_indices
)
{
	for (unsigned i = 0; i < constraints.Size(); ++i) {
		VariantMap* var_map = constraints[i].GetVariantMapPtr();
		VariantVector* shapeop_vertices = (*var_map)["vertices"].GetVariantVectorPtr();

		for (unsigned j = 0; j < shapeop_vertices->Size(); ++j) {
			VariantMap* var_map2 = (*shapeop_vertices)[j].GetVariantMapPtr();
			int current_raw_index = (*var_map2)["raw_index"].GetInt();
			int processed_index = new_indices[current_raw_index];
			SetConstraintProcessedIndex(constraints[i], j, processed_index);
		}
	}
}

// ShapeOp's solver natively operates on raw points not meshes.
// This struct helps track meshes across a ShapeOp simulation.
struct MeshTrackingData {
	Variant* mesh;
	std::vector<int> raw_indices;
	std::vector<int> processed_indices;
};

void SetUpMeshTrackingData(
	VariantVector& unverified_meshes,
	const Vector<Vector3>& raw_vertices,
	std::vector<MeshTrackingData>& tracked_meshes
)
{
	for (int i = 0; i < unverified_meshes.Size(); ++i) {
		if (TriMesh_Verify(unverified_meshes[i])) {
			MeshTrackingData mtd;
			mtd.mesh = &unverified_meshes[i];
			VariantMap* var_map = unverified_meshes[i].GetVariantMapPtr();
			VariantVector* vertex_list = (*var_map)["vertices"].GetVariantVectorPtr();
			for (unsigned j = 0; j < vertex_list->Size(); ++j) {
				Vector3 v = (*vertex_list)[j].GetVector3();
				bool found = false;
				for (unsigned k = 0; k < raw_vertices.Size(); ++k) {
					Vector3 w = raw_vertices[k];
					if (
						v.x_ == w.x_ &&
						v.y_ == w.y_ &&
						v.z_ == w.z_
						)
					{
						found = true;
						mtd.raw_indices.push_back(k);
						break;
					}
				}
				if (!found) {
					std::cout << "FAILED to find exact match for tracked mesh vertex in raw_vertices" << std::endl;
				}
			}
			if ((unsigned)mtd.raw_indices.size() == vertex_list->Size()) {
				// found a match for every vertex, so we can track this mesh
				tracked_meshes.push_back(mtd);
			}
		}
	}
}

void UpdateTrackedMeshes(
	const std::vector<MeshTrackingData>& tracked_meshes,
	const Vector<int>& new_indices,
	const VariantVector& points,
	VariantVector& meshes_out
)
{
	for (int i = 0; i < tracked_meshes.size(); ++i) {
		MeshTrackingData mtd = tracked_meshes[i];
		for (int j = 0; j < mtd.raw_indices.size(); ++j) {
			int raw_index = mtd.raw_indices[j];
			mtd.processed_indices.push_back(new_indices[raw_index]);
		}
		VariantVector new_vertex_list;
		for (int j = 0; j < mtd.processed_indices.size(); ++j) {
			//Vector3 new_vertex = welded_vertices[mtd.processed_indices[j]];
			Vector3 new_vertex = points[mtd.processed_indices[j]].GetVector3();
			new_vertex_list.Push(new_vertex);
		}
		VariantVector new_face_list = TriMesh_GetFaceList(*mtd.mesh);
		Variant new_mesh = TriMesh_Make(new_vertex_list, new_face_list);
		meshes_out.Push(new_mesh);
	}
}

} // namespace

String ShapeOp_Solve::iconTexture = "Textures/Icons/DefaultIcon.png";

ShapeOp_Solve::ShapeOp_Solve(Context* context) : IoComponentBase(context, 8, 2)
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

	inputSlots_[2]->SetName("Gravity");
	inputSlots_[2]->SetVariableName("G");
	inputSlots_[2]->SetDescription("Gravity");
	inputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[3]->SetName("Mass");
	inputSlots_[3]->SetVariableName("M");
	inputSlots_[3]->SetDescription("Mass (assigned to all points; 1.0f default)");
	inputSlots_[3]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(Variant(1.0f));
	inputSlots_[3]->DefaultSet();

	inputSlots_[4]->SetName("Damping");
	inputSlots_[4]->SetVariableName("D");
	inputSlots_[4]->SetDescription("Damping (default 1.0f)");
	inputSlots_[4]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[4]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[4]->SetDefaultValue(Variant(1.0f));
	inputSlots_[4]->DefaultSet();

	inputSlots_[5]->SetName("Timestep");
	inputSlots_[5]->SetVariableName("T");
	inputSlots_[5]->SetDescription("Timestep (default 1.0f)");
	inputSlots_[5]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[5]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[5]->SetDefaultValue(Variant(1.0f));
	inputSlots_[5]->DefaultSet();

	inputSlots_[6]->SetName("Iterations");
	inputSlots_[6]->SetVariableName("I");
	inputSlots_[6]->SetDescription("Iterations for solver (default 10)");
	inputSlots_[6]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[6]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[6]->SetDefaultValue(Variant(10));
	inputSlots_[6]->DefaultSet();

	inputSlots_[7]->SetName("MeshList");
	inputSlots_[7]->SetVariableName("ML");
	inputSlots_[7]->SetDescription("List of tracked meshes");
	inputSlots_[7]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[7]->SetDataAccess(DataAccess::LIST);

	outputSlots_[0]->SetName("Points");
	outputSlots_[0]->SetVariableName("Pts");
	outputSlots_[0]->SetDescription("Point list output");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[1]->SetName("MeshList");
	outputSlots_[1]->SetVariableName("ML");
	outputSlots_[1]->SetDescription("Mesh list output");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[1]->SetDataAccess(DataAccess::LIST);
}

void ShapeOp_Solve::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	///////////////////////////////////////////////////////////////////////////////
	// I. EXTRACT AND VERIFY INPUTS
	///////////////////////////////////////////////////////////////////////////////

	// get weld epsilon
	float weld_eps = inSolveInstance[1].GetFloat();
	if (weld_eps <= 0.0f) {
		URHO3D_LOGWARNING("ShapeOp_Solve --- WeldDist must be > 0.0f");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	// get gravity
	Variant g_var = inSolveInstance[2];
	bool add_gravity = false;
	Vector3 g_vec;
	if (g_var.GetType() == VAR_VECTOR3) {
		add_gravity = true;
		g_vec = g_var.GetVector3();
	}

	// get ShapeOp solver parameters
	double mass = 1.0, damping = 1.0, timestep = 1.0;
	int iterations = 10;
	GetInitializationParameters(
		inSolveInstance[3], mass,
		inSolveInstance[4], damping,
		inSolveInstance[5], timestep,
		inSolveInstance[6], iterations
	);

	// get all valid constraints
	VariantVector unverified_constraints = inSolveInstance[0].GetVariantVector();
	VariantVector constraints;
	for (unsigned i = 0; i < unverified_constraints.Size(); ++i) {

		Variant var = unverified_constraints[i];
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

	// Collect points as raw_vertices, weld nearby ones, update constraint metadata
	Vector<Vector3> raw_vertices;
	SetUpRawVertices(constraints, raw_vertices);
	Vector<Vector3> welded_vertices;
	Vector<int> new_indices;
	WeldVertices(raw_vertices, welded_vertices, new_indices, 0.001f);
	UpdateConstraintsAfterWelding(constraints, new_indices);

	VariantVector unverified_meshes = inSolveInstance[7].GetVariantVector();
	std::vector<MeshTrackingData> tracked_meshes;
	SetUpMeshTrackingData(unverified_meshes, raw_vertices, tracked_meshes);

	///////////////////////////////////////////////////////////////////////////////
	// II. ShapeOp API calls
	///////////////////////////////////////////////////////////////////////////////

	// 1) Create the solver with #shapeop_create

	ShapeOpSolver* op = shapeop_create();

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

	// 3A) Setup the constraints with #shapeop_addConstraint and #shapeop_editConstraint

	// all ids are captured here so their data won't go out of scope after added to the solver
	// since they must be added as array ptr/size combos
	std::vector<std::vector<int> > all_ids;
	for (unsigned i = 0; i < constraints.Size(); ++i) {

		Variant constraint = constraints[i];
		std::vector<int> ids;
		int val = GetConstraintIds(constraint, ids);
		all_ids.push_back(ids);
	}

	assert((unsigned)all_ids.size() == constraints.Size());

	int count = 0;
	for (unsigned i = 0; i < constraints.Size(); ++i) {

		Variant constraint = constraints[i];
		int nb_ids = (int)all_ids[i].size();

		shapeop_addConstraint(
			op,
			ShapeOpConstraint_constraintType(constraint).CString(),
			all_ids[i].data(),
			nb_ids,
			ShapeOpConstraint_weight(constraint)
		);
		count++;
	}

	URHO3D_LOGINFO("ShapeOp_Solve --- " + String(count) + " Constraints added");

	// 3B) Setup the forces with #shapeop_addVertexForce

	// ... not sure if we're still adding forces to individual vertices

	double gravity_force[3] = { (double)g_vec.x_, (double)g_vec.y_, (double)g_vec.z_ };
	if (add_gravity) {
		shapeop_addGravityForce(op, gravity_force);
		URHO3D_LOGINFO("ShapeOp_Solve --- Gravity force added");
	}

	// 4) Initialize the solver with #shapeop_init or #shapeop_initDynamic

	//shapeop_init(op);
	shapeop_initDynamic(op, mass, damping, timestep);

	// 5) Optimize with #shapeop_solve

	URHO3D_LOGINFO("ShapeOp_Solve --- solver starting for " + String(iterations) + " iterations....");
	shapeop_solve(op, iterations);
	URHO3D_LOGINFO("ShapeOp_Sovle --- solver finished");

	// 6) Get back the vertices with #shapeop_getPoints

	std::vector<double> pts_out(3 * nb_points);
	shapeop_getPoints(op, pts_out.data(), nb_points);

	// 7) Delete the solver with #shapeop_delete

	shapeop_delete(op);

	///////////////////////////////////////////////////////////////////////////////
	// III. Prepare and assign outputs
	///////////////////////////////////////////////////////////////////////////////

	VariantVector points;
	for (int i = 0; i < 3 * nb_points; i += 3) {
		Vector3 pt((float)pts_out[i], (float)pts_out[i + 1], (float)pts_out[i + 2]);
		points.Push(pt);
	}

	VariantVector meshes_out;
	UpdateTrackedMeshes(tracked_meshes, new_indices, points, meshes_out);

	outSolveInstance[0] = points;
	outSolveInstance[1] = meshes_out;
}