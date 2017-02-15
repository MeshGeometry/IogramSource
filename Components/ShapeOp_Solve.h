#pragma once

#include "IoComponentBase.h"

#include <vector>

#include "ShapeOp_API.h"

// ShapeOp's solver natively operates on raw points not meshes.
// This struct helps track meshes across a ShapeOp simulation.
struct MeshTrackingData {
	Urho3D::Variant mesh;
	std::vector<int> raw_indices;
	std::vector<int> processed_indices;
};

class URHO3D_API ShapeOp_Solve : public IoComponentBase {
	URHO3D_OBJECT(ShapeOp_Solve, IoComponentBase)
public:
	ShapeOp_Solve(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	void AddInputSlot() = delete;
	void AddOutputSlot() = delete;
	void DeleteInputSlots(int index) = delete;
	void DeleteOutputSlot(int index) = delete;

	static Urho3D::String iconTexture;

	// state parameters
	ShapeOpSolver* op = NULL;
	int m_iterations = 10;
	int m_nb_points = -1;
	std::vector<MeshTrackingData> m_tracked_meshes;
	Urho3D::Vector<int> m_new_indices;

};