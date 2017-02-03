#include "ShapeOp_MakeVertexForce.h"

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

String ShapeOp_MakeVertexForce::iconTexture = "Textures/Icons/DefaultIcon.png";

ShapeOp_MakeVertexForce::ShapeOp_MakeVertexForce(Context* context) : IoComponentBase(context, 3, 3)
{
	SetName("ShapeOpMakeVertexForce");
	SetFullName("ShapeOp Make Vertex Force");
	SetDescription("...");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh input");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh input");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Forces");
	inputSlots_[1]->SetVariableName("Fs");
	inputSlots_[1]->SetDescription("Forces");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDataAccess(DataAccess::LIST);
	
	inputSlots_[2]->SetName("Vertices");
	inputSlots_[2]->SetVariableName("Vs");
	inputSlots_[2]->SetDescription("Vertices");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDataAccess(DataAccess::LIST);

	outputSlots_[0]->SetName("VertexForces");
	outputSlots_[0]->SetVariableName("VFs");
	outputSlots_[0]->SetDescription("List of Vertex Forces");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[1]->SetName("Forces");
	outputSlots_[1]->SetVariableName("Fs");
	outputSlots_[1]->SetDescription("Forces");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[1]->SetDataAccess(DataAccess::LIST);

	outputSlots_[2]->SetName("Vertices");
	outputSlots_[2]->SetVariableName("Vs");
	outputSlots_[2]->SetDescription("Vertices");
	outputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[2]->SetDataAccess(DataAccess::LIST);
}

void ShapeOp_MakeVertexForce::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	// Most of this component's work is input verification
	if (!IsAllInputValid(inSolveInstance)) {
		URHO3D_LOGWARNING("ShapeOp_MakeConstraint --- inputs not of correct VariantType");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	// input 0

	Variant mesh_in = inSolveInstance[0];
	if (!TriMesh_Verify(mesh_in)) {
		URHO3D_LOGWARNING("Mesh input must be a TriMesh");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	VariantVector vertex_list = TriMesh_GetVertexList(mesh_in);
	int nb_points = (int)vertex_list.Size();

	// input 1

	if (inSolveInstance[1].GetType() != Urho3D::VariantType::VAR_VARIANTVECTOR) {
		URHO3D_LOGWARNING("ShapeOp_MakeVertexForce --- Forces must be a list of Vector3's");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	VariantVector force_list = inSolveInstance[1].GetVariantVector();
	for (unsigned i = 0; i < force_list.Size(); ++i) {
		if (force_list[i].GetType() != Urho3D::VariantType::VAR_VECTOR3) {
			URHO3D_LOGWARNING("ShapeOp_MakeVertexForce --- Each element of Forces must be a Vector3");
			SetAllOutputsNull(outSolveInstance);
			return;
		}
	}

	// input 2

	if (inSolveInstance[2].GetType() != Urho3D::VariantType::VAR_VARIANTVECTOR) {
		URHO3D_LOGWARNING("ShapeOp_MakeVertexForce --- Vertices must be a list of integer ids");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	VariantVector id_list = inSolveInstance[2].GetVariantVector();
	for (unsigned i = 0; i < id_list.Size(); ++i) {
		if (id_list[i].GetType() != Urho3D::VariantType::VAR_INT) {
			URHO3D_LOGWARNING("ShapeOp_MakeVertexForce --- Each element of Vertices must be an integer id");
			SetAllOutputsNull(outSolveInstance);
			return;
		}
	}

	if (force_list.Size() != id_list.Size()) {
		URHO3D_LOGWARNING("ShapeOp_MakeVertexForce --- Forces and Vertices lists must be the same length");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	VariantVector vertexForceList;
	for (unsigned i = 0; i < id_list.Size(); ++i) {
		int id = id_list[i].GetInt();
		Vector3 v = force_list[i].GetVector3();
		Variant vertexForce = ShapeOpVertexForce_Make(v, id);
		vertexForceList.Push(vertexForce);
	}

	outSolveInstance[0] = vertexForceList;
	outSolveInstance[1] = force_list;
	outSolveInstance[2] = vertex_list;
}