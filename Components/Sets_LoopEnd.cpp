#include "Sets_LoopEnd.h"
#include "Sets_LoopBegin.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Scene/SceneEvents.h>

using namespace Urho3D;

String Sets_LoopEnd::iconTexture = "Textures/Icons/Sets_LoopEnd.png";

Sets_LoopEnd::Sets_LoopEnd(Urho3D::Context* context) : IoComponentBase(context, 0,0)
{
	SetName("LoopEnd");
	SetFullName("Loop End");
	SetDescription("Ends a loop or transmits data back to start.");

	AddInputSlot(
		"LoopStart",
		"LS",
		"Loop start component to operate on. Connect to any Loop Start output.",
		VAR_NONE,
		ITEM
	);

	AddInputSlot(
		"Stop",
		"S",
		"Optional bool to stop loop",
		VAR_BOOL,
		ITEM,
		false
	);

	AddInputSlot(
		"Data",
		"D",
		"Generic data to transmit back to start",
		VAR_FLOAT,
		ITEM
	);

	AddOutputSlot(
		"Last Data",
		"D",
		"Last computed data",
		VAR_NONE,
		ITEM
	);
}

void Sets_LoopEnd::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	//find the upstream loop start
	Urho3D::Pair<Urho3D::SharedPtr<IoComponentBase>, int> linkIn = GetIncomingLink(0);

	if (linkIn.first_.Null())
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//check that it is of type LoopBegin
	if (linkIn.first_->GetTypeName() == "Sets_LoopBegin")
	{
		Sets_LoopBegin* loopBegin = (Sets_LoopBegin*)linkIn.first_.Get();
		if (loopBegin) //double check
		{
			//optionally stop the loop
			bool stop = inSolveInstance[1].GetBool();
			if (stop)
			{
				//need a good way to stop
				loopBegin->UnsubscribeFromEvent(E_SCENEUPDATE);
				//loopBegin->currentIndex = loopBegin->numSteps;
			}
			else {
				//transmit the data backwards!! Here be dragons.
				loopBegin->trackedData = inSolveInstance[2];
			}

			outSolveInstance[0] = inSolveInstance[2];
		}
	}
	else
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}
}