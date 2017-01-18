#include "Scene_PlayAnimation.h"
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/AnimationState.h>

#include <Urho3D/Resource/ResourceCache.h>
#include "IoGraph.h"

using namespace Urho3D;

String Scene_PlayAnimation::iconTexture = "Textures/Icons/Scene_PlayAnimation.png";

Scene_PlayAnimation::Scene_PlayAnimation(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("PlayAnimation");
	SetFullName("Play animation");
	SetDescription("Play an animation");

	inputSlots_[0]->SetName("NodeID");
	inputSlots_[0]->SetVariableName("ID");
	inputSlots_[0]->SetDescription("ID of node");
	inputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("AnimationController");
	inputSlots_[1]->SetVariableName("AC");
	inputSlots_[1]->SetDescription("Pointer to animation contorller");
	inputSlots_[1]->SetVariantType(VariantType::VAR_PTR);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[2]->SetName("Animation");
	inputSlots_[2]->SetVariableName("A");
	inputSlots_[2]->SetDescription("Path to animation");
	inputSlots_[2]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("Animated Model");
	outputSlots_[0]->SetVariableName("AM");
	outputSlots_[0]->SetDescription("Pointer to animated Model");
	outputSlots_[0]->SetVariantType(VariantType::VAR_PTR); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}


void Scene_PlayAnimation::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	AnimationController* ac = (AnimationController*)inSolveInstance[1].GetPtr();

	if (!ac) {
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	ResourceCache* rc = GetSubsystem<ResourceCache>();
	String ani = inSolveInstance[2].GetString();
	ac->PlayExclusive(ani, 0, true, 0.2f);


}
