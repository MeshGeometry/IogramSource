#include "ShapeOp_Gravity.h"

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

String ShapeOp_Gravity::iconTexture = "Textures/Icons/DefaultIcon.png";

ShapeOp_Gravity::ShapeOp_Gravity(Context* context) : IoComponentBase(context, 3, 3)
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

void ShapeOp_Gravity::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
}