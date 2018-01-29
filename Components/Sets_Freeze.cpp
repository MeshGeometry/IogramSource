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


#include "Sets_Freeze.h"
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Compression.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>;
#include <Urho3D/UI/UIEvents.h>;
#include "ColorDefs.h"
#include "IoGraph.h"
#include <assert.h>

using namespace Urho3D;

String Sets_Freeze::iconTexture = "Textures/Icons/Sets_Freeze.png";

Sets_Freeze::Sets_Freeze(Urho3D::Context* context) : IoComponentBase(context, 0, 0),
freeze_(false)
{
	SetName("Freeze");
	SetFullName("Data Freeze");
	SetDescription("Writes all incoming data to a file. Then, if enabled, reads that data back. Good for saving states.");

	AddInputSlot(
		"Data",
		"D",
		"Data to record",
		VAR_STRING,
		TREE
	);

	AddOutputSlot(
		"Output",
		"O",
		"Output Data",
		VAR_VARIANTVECTOR,
		LIST
	);

	//init file and resource name
	resourceName_ = "Graphs/FreezeFile_" + ID + ".data";
	filename_ = "Data/" + resourceName_;
}

void Sets_Freeze::PreLocalSolve()
{
	freeze_ = GetGenericData("Freeze").GetBool();
	String cachedName = GetGenericData("FreezeFile").GetString();
	if (!cachedName.Empty())
	{
		resourceName_ = cachedName;
		filename_ = "Data/" + resourceName_;
	}
}

void Sets_Freeze::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	if (!freeze_)
	{
		//write to disk
		File* file = new File(GetContext(), filename_, FileMode::FILE_WRITE);
		VariantMap map = inSolveInstance[0].GetVariantMap();
		if (file->GetMode() == FILE_WRITE)
		{
			//file->Write(msg.GetData(), msg.GetSize());
			file->WriteVariantMap(map);
		}
		file->Close();

		//TODO: suppor variant map tree output.
		VariantVector listOut;
		VariantMap::Iterator itr = map.Begin();
		for (itr = map.Begin(); itr != map.End(); itr++)
		{
			if (itr->second_.GetType() == VAR_VARIANTVECTOR)
			{
				VariantVector vec = itr->second_.GetVariantVector();
				listOut.Push(vec);
			}
		}

		//push to output
		outSolveInstance[0] = listOut;
		return;
	}
	else
	{
		//get file via resource cache
		ResourceCache* rc = GetSubsystem<ResourceCache>();
		SharedPtr<File> file = rc->GetFile(resourceName_);
		if (file)
		{			
			VariantMap vm = file->ReadVariantMap();

			//create flat list.
			//TODO: maintain tree structure
			VariantVector listOut;
			VariantMap::Iterator itr = vm.Begin();
			for (itr = vm.Begin(); itr != vm.End(); itr++)
			{
				if (itr->second_.GetType() == VAR_VARIANTVECTOR)
				{
					VariantVector vec = itr->second_.GetVariantVector();
					listOut.Push(vec);
				}
			}

			file->Close();

			outSolveInstance[0] = listOut;
			return;	
		}
		else
		{
			SetAllOutputsNull(outSolveInstance);
			return;
		}
	}
}

void Sets_Freeze::HandleCustomInterface(Urho3D::UIElement* customElement)
{
	Button* b = customElement->CreateChild<Button>();
	b->SetSelected(true);
	b->SetStyleAuto();
	freeze_ ? b->SetColor(LIGHT_GREEN) : b->SetColor(BRIGHT_ORANGE);
	b->SetMinHeight(24);
	
	Text* t = b->CreateChild<Text>();
	t->SetStyleAuto();
	t->SetAlignment(HA_CENTER, VA_CENTER);
	t->SetColor(Color::BLACK);
	freeze_ ? t->SetText("Writing...") : t->SetText("Reading...");

	SubscribeToEvent(b, E_PRESSED, URHO3D_HANDLER(Sets_Freeze, HandleModeChange));
}

void Sets_Freeze::HandleModeChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace Pressed;
	Button* b = (Button*)eventData[P_ELEMENT].GetPtr();
	if (b)
	{
		if (b->IsSelected())
		{
			b->SetColor(BRIGHT_ORANGE);
			Text* t = (Text*)b->GetChild(0);
			if (t)
			{
				t->SetText("Reading...");
			}
			freeze_ = true;
			b->SetSelected(false);
		}
		else
		{
			b->SetColor(LIGHT_GREEN);
			Text* t = (Text*)b->GetChild(0);
			if (t)
			{
				t->SetText("Writing...");
			}
			freeze_ = false;
			b->SetSelected(true);
		}
		SetGenericData("Freeze", freeze_);
		SetGenericData("FreezeFile", resourceName_);
	}

	solvedFlag_ = 0;
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}

void Sets_Freeze::HandleNameChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{

}
