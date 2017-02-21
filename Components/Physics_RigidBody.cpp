#include "Physics_RigidBody.h"
#include "IoGraph.h"
#include "Urho3D/Physics/PhysicsWorld.h"
#include "Urho3D/Physics/RigidBody.h"
#include "Urho3D/Physics/CollisionShape.h"
#include "Urho3D/Graphics/StaticModel.h"

using namespace Urho3D;

String Physics_RigidBody::iconTexture = "Textures/Icons/Physics_RigidBody.png";

Physics_RigidBody::Physics_RigidBody(Urho3D::Context* context) : IoComponentBase(context, 4, 2)
{
	SetName("RigidBody");
	SetFullName("Rigid Body");
	SetDescription("Adds rigid body behaviour to a node");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("PHYSICS");

	inputSlots_[0]->SetName("Node ID");
	inputSlots_[0]->SetVariableName("ID");
	inputSlots_[0]->SetDescription("Node ID to add Rigid body to.");
	inputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Physics World");
	inputSlots_[1]->SetVariableName("PW");
	inputSlots_[1]->SetDescription("Physics World");
	inputSlots_[1]->SetVariantType(VariantType::VAR_PTR);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[2]->SetName("Mass");
	inputSlots_[2]->SetVariableName("M");
	inputSlots_[2]->SetDescription("Mass. Set to zero if you want a fixed object");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(10.0f);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("Shape type");
	inputSlots_[3]->SetVariableName("S");
	inputSlots_[3]->SetDescription("Collision shape type.");
	inputSlots_[3]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue("Box");
	inputSlots_[3]->DefaultSet();


	outputSlots_[0]->SetName("Rigid Body");
	outputSlots_[0]->SetVariableName("RB");
	outputSlots_[0]->SetDescription("Pointer to Rigid Body");
	outputSlots_[0]->SetVariantType(VariantType::VAR_PTR);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Collision Shape");
	outputSlots_[1]->SetVariableName("CS");
	outputSlots_[1]->SetDescription("Pointer to Collision Shape");
	outputSlots_[1]->SetVariantType(VariantType::VAR_PTR);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Physics_RigidBody::PreLocalSolve()
{
	Scene* scene = GetSubsystem<IoGraph>()->scene;
	for (int i = 0; i < trackedItems.Size(); i++)
	{
		Component* sm = scene->GetComponent(trackedItems[i]);
		if (sm != NULL)
		{
			sm->Remove();
		}
	}

	trackedItems.Clear();
}

void Physics_RigidBody::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{

	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();

	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	int id = inSolveInstance[0].GetInt();
	Node* node = scene->GetNode(id);

	if (!node)
	{
		URHO3D_LOGERROR("Could not find node: " + String(id));
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	RigidBody* rb = node->CreateComponent<RigidBody>();
	rb->SetMass(inSolveInstance[2].GetFloat());
	CollisionShape* cs = node->CreateComponent<CollisionShape>();
	String shapeTypeName = inSolveInstance[3].GetString();
	StaticModel* sm = node->GetComponent<StaticModel>();
	Vector3 size = Vector3::ONE;
	if (sm)
	{
		size = sm->GetBoundingBox().Size();
	}
	if (shapeTypeName == "Box")
	{
		cs->SetBox(sm->GetBoundingBox().Size()); //TODO need to find bounding box
	}

	if (shapeTypeName == "Sphere")
	{
		cs->SetSphere(sm->GetBoundingBox().HalfSize().Length());
	}

	if (shapeTypeName == "Convex")
	{
		if (sm)
		{
			cs->SetConvexHull(sm->GetModel());
		}
	}

	trackedItems.Push(rb->GetID());
	trackedItems.Push(cs->GetID());

	outSolveInstance[0] = rb;
	outSolveInstance[1] = cs;
}
