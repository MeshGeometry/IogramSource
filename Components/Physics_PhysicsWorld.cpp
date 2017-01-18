#include "Physics_PhysicsWorld.h"
#include "IoGraph.h"
#include "Urho3D/Physics/PhysicsWorld.h"

using namespace Urho3D;

String Physics_PhysicsWorld::iconTexture = "Textures/Icons/Physics_PhysicsWorld.png";

Physics_PhysicsWorld::Physics_PhysicsWorld(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("PhysicsWorld");
	SetFullName("PHysics World");
	SetDescription("Initializes physics world");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("PHYSICS");

	inputSlots_[0]->SetName("Step");
	inputSlots_[0]->SetVariableName("Go");
	inputSlots_[0]->SetDescription("Run physics");
	inputSlots_[0]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(false);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Gravity");
	inputSlots_[1]->SetVariableName("G");
	inputSlots_[1]->SetDescription("Gravity Vector");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Vector3(0, -9.81, 0));
	inputSlots_[1]->DefaultSet();


	outputSlots_[0]->SetName("Physics World");
	outputSlots_[0]->SetVariableName("PW");
	outputSlots_[0]->SetDescription("Pointer to static physics world");
	outputSlots_[0]->SetVariantType(VariantType::VAR_PTR); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Physics_PhysicsWorld::PreLocalSolve()
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

void Physics_PhysicsWorld::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{

	Scene* scene = GetSubsystem<IoGraph>()->scene;
	
	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered.");
		return;
	}

	PhysicsWorld* pw = scene->CreateComponent<PhysicsWorld>();
	pw->SetGravity(inSolveInstance[1].GetVector3());
	pw->SetUpdateEnabled(inSolveInstance[0].GetBool());

	trackedItems.Push(pw->GetID());

	outSolveInstance[0] = pw;
}
