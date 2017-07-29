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


#include "Sets_Heap.h"
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <assert.h>

using namespace Urho3D;

String Sets_Heap::iconTexture = "Textures/Icons/Sets_Heap.png";

Sets_Heap::Sets_Heap(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("DataRecorder");
	SetFullName("Data Recorder");
	SetDescription("Records generic data and outputs to list.");

	AddInputSlot(
		"DataItem",
		"D",
		"Data to record",
		VAR_STRING,
		TREE
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

void Sets_Heap::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	bool reset = inSolveInstance[2].GetBool();
	int limit = inSolveInstance[1].GetInt();


	if (reset)
	{
		trackedItems.Clear();
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	else 
	{
		//push to tracked items
		VariantMap allData = inSolveInstance[0].GetVariantMap();
		VariantMap::Iterator itr = allData.Begin();
		for (itr = allData.Begin(); itr != allData.End(); itr++)
		{
			if (itr->second_.GetType() == VAR_VARIANTVECTOR)
			{
				VariantVector vec = itr->second_.GetVariantVector();
				trackedItems.Push(vec);

				while (trackedItems.Size() > limit)
				{
					trackedItems.Erase(trackedItems.Begin());
				}
			}
		}

		//output
		outSolveInstance[0] = trackedItems;
		return;
	}



}
