//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "Sets_Pop.h"
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/IO/FileSystem.h>
#include "IoGraph.h"
#include <assert.h>

using namespace Urho3D;

String Sets_Pop::iconTexture = "Textures/Icons/Sets_Pop.png";

Sets_Pop::Sets_Pop(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("DataRecorder");
	SetFullName("Data Recorder");
	SetDescription("Records generic data and outputs to list.");

	AddInputSlot(
		"List",
		"L",
		"List to pop.",
		VAR_FLOAT,
		LIST
	);

	AddInputSlot(
		"Rate",
		"R",
		"Rate to pop items",
		VAR_FLOAT,
		ITEM,
		0.1f
	);

	AddInputSlot(
		"Stride",
		"S",
		"Number of items to pop",
		VAR_INT,
		ITEM,
		1
	);

	AddOutputSlot(
		"Pop",
		"P",
		"Popped items",
		VAR_FLOAT,
		LIST
	);

	SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(Sets_Pop, HandleUpdate));
}

void Sets_Pop::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	
	VariantVector list = inSolveInstance[0].GetVariantVector();
	if (!list.Empty())
	{
		trackedItems.Push(list);
	}

	if (!trackedItems.Empty())
	{
		SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(Sets_Pop, HandleUpdate));
	}

	rate_ = inSolveInstance[1].GetFloat();
	stride_ = inSolveInstance[2].GetInt();
	counter_ = 0.0f;

}

void Sets_Pop::HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace SceneUpdate;

	float step = eventData[P_TIMESTEP].GetFloat();
	counter_ += step;
	if (counter_ >= rate_)
	{
		//push stuff to output slot
		VariantVector subList;
		while (subList.Size() < stride_)
		{
			subList.Push(trackedItems.Front());
			trackedItems.Erase(trackedItems.Begin());
		}

		IoDataTree tree(GetContext(), subList);
		outputSlots_[0]->SetIoDataTree(tree);

		//solve
		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();

		counter_ = 0.0f;
	}

	//don't call update if nothing to do
	if (trackedItems.Empty())
	{
		UnsubscribeFromEvent(E_SCENEUPDATE);
	}

}
