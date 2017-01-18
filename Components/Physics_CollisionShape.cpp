#include "Physics_CollisionShape.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Physics_CollisionShape::iconTexture = "Textures/Icons/Physics_CollisionShape.png";

void Physics_CollisionShape::PreLocalSolve()
{
	
}


Physics_CollisionShape::Physics_CollisionShape(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("CollisionShape");
	SetFullName("Collision Shape");
	SetDescription("Construct a collision shape form a mesh or model");

	inputSlots_[0]->SetName("Node ID");
	inputSlots_[0]->SetVariableName("ID");
	inputSlots_[0]->SetDescription("Node to construct collision shape on");
	inputSlots_[0]->SetVariantType(VariantType::VAR_INT);

	inputSlots_[1]->SetName("Base Mesh");
	inputSlots_[1]->SetVariableName("M");
	inputSlots_[1]->SetDescription("Mesh to construct collision shape from");
	inputSlots_[1]->SetVariantType(VariantType::VAR_PTR);

	inputSlots_[2]->SetName("Shape type");
	inputSlots_[2]->SetVariableName("T");
	inputSlots_[2]->SetDescription("Type of collision shape to create");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);	


	outputSlots_[0]->SetName("RigidBody");
	outputSlots_[0]->SetVariableName("R");
	outputSlots_[0]->SetDescription("Rigid Body");
	outputSlots_[0]->SetVariantType(VariantType::VAR_PTR);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Physics_CollisionShape::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{


}