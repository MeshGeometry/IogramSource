#include "Sets_LoopBegin.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Scene/Scene.h>

#include "IoGraph.h"

using namespace Urho3D;

String Sets_LoopBegin::iconTexture = "Textures/Icons/Sets_LoopBegin.png";

Sets_LoopBegin::Sets_LoopBegin(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("ForLoopBegin");
	SetFullName("For Loop Begin");
	SetDescription("Entry point for a For Loop");

	AddInputSlot(
		"Name",
		"L",
		"Optional label for this loop.",
		VAR_STRING,
		ITEM,
		"GenericLoop"
	);

	AddInputSlot(
		"Steps",
		"N",
		"Number of steps to compute. -1 for indefinite loop",
		VAR_INT,
		ITEM,
		10
	);

	AddInputSlot(
		"Data",
		"D",
		"Initial Data",
		VAR_FLOAT,
		ITEM
	);

	AddOutputSlot(
		"Index",
		"I",
		"Current index",
		VAR_INT,
		ITEM
	);

	AddOutputSlot(
		"Data",
		"D",
		"Optional temporary data",
		VAR_FLOAT,
		ITEM
	);
}

void Sets_LoopBegin::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	String loopName = inSolveInstance[0].GetString();
	int userSteps = inSolveInstance[1].GetInt();

	//reset behaviour
	if (userSteps != numSteps || loopName != loopID)
	{
		numSteps = userSteps;
		loopID = loopName;
		currentIndex = 0;
		trackedData = inSolveInstance[2];
		SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(Sets_LoopBegin, HandleUpdate));
		outSolveInstance[0] = currentIndex;
		outSolveInstance[1] = trackedData;
	}
	else //loop behaviour
	{
		currentIndex++;
		outSolveInstance[0] = currentIndex;
		outSolveInstance[1] = trackedData;
	}
}

void Sets_LoopBegin::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	
	//only update if this condition is met
	if (currentIndex < numSteps)
	{
		solvedFlag_ = 0;
		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}
	else
	{
		UnsubscribeFromEvent(E_SCENEUPDATE);
	}
}