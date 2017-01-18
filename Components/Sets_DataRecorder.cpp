#include "Sets_DataRecorder.h"

#include <assert.h>

using namespace Urho3D;

String Sets_DataRecorder::iconTexture = "Textures/Icons/Sets_DataRecorder.png";

Sets_DataRecorder::Sets_DataRecorder(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("DataRecorder");
	SetFullName("Data Recorder");
	SetDescription("Records generic data and outputs to list.");

	AddInputSlot(
		"DataItem",
		"D",
		"Data to record",
		VAR_STRING,
		ITEM
	);

	AddInputSlot(
		"RecordLimit",
		"N",
		"Number of items to record",
		VAR_INT,
		ITEM,
		100
	);

	AddInputSlot(
		"Reset",
		"R",
		"Reset the recording",
		VAR_BOOL,
		ITEM,
		false
	);

	AddOutputSlot(
		"Data",
		"D",
		"Optional temporary data",
		VAR_FLOAT,
		LIST
	);
}

void Sets_DataRecorder::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	bool reset = inSolveInstance[2].GetBool();
	int limit = inSolveInstance[1].GetInt();

	if (reset)
	{
		trackedItems.Clear();
	}
	else
	{
		trackedItems.Push(inSolveInstance[0]);
		if (trackedItems.Size() > limit)
		{
			trackedItems.Erase(trackedItems.Begin());
		}
	}

	if (trackedItems.Empty())
	{
		trackedItems.Push(Variant());
	}

	outSolveInstance[0] = trackedItems;
}
