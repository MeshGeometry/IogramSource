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


#include "Scene_AppendRenderPath.h"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Renderer.h>

using namespace Urho3D;

String Scene_AppendRenderPath::iconTexture = "Textures/Icons/Scene_AppendRenderPath.png";

Scene_AppendRenderPath::Scene_AppendRenderPath(Urho3D::Context* context) : IoComponentBase(context, 4, 1)
{
	SetName("RenderPath");
	SetFullName("Render Path");
	SetDescription("Appends a render path item to a viewport.");

	inputSlots_[0]->SetName("RenderPath");
	inputSlots_[0]->SetVariableName("RP");
	inputSlots_[0]->SetDescription("Path to render path definition");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	//inputSlots_[0]->SetDefaultValue("PostProcess/Blur.xml");
	//inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("ViewportID");
	inputSlots_[1]->SetVariableName("VP");
	inputSlots_[1]->SetDescription("Viewport ID to add render path to.");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(0);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Parameters");
	inputSlots_[2]->SetVariableName("P");
	inputSlots_[2]->SetDescription("Set additional render path parameters");
	inputSlots_[2]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[2]->SetDataAccess(DataAccess::LIST);


	inputSlots_[3]->SetName("Values");
	inputSlots_[3]->SetVariableName("V");
	inputSlots_[3]->SetDescription("Addtional render path values");
	inputSlots_[3]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[3]->SetDataAccess(DataAccess::LIST);


	outputSlots_[0]->SetName("Tags");
	outputSlots_[0]->SetVariableName("A");
	outputSlots_[0]->SetDescription("Append Render path tags");
	outputSlots_[0]->SetVariantType(VariantType::VAR_STRING); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Scene_AppendRenderPath::PreLocalSolve()
{
	Renderer* renderer = GetSubsystem<Renderer>();
	VariantVector vpList = GetGlobalVar("ViewportVector").GetVariantVector();
	int numVP = vpList.Size();

	for (int i = 0; i < trackedItems.Size(); i++)
	{
		int vpID = trackedItems[i].first_;
		String rpCommand = trackedItems[i].second_;

		if (vpID < numVP)
		{
			Viewport* vp = (Viewport*)vpList[vpID].GetPtr();
			if (vp)
			{
				RenderPath* renderPath =vp->GetRenderPath();
				renderPath->RemoveCommands(rpCommand);
				renderPath->RemoveRenderTargets(rpCommand);
			}

		}

	}

	trackedItems.Clear();
}

void Scene_AppendRenderPath::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	int vId = inSolveInstance[1].GetInt();
	
	Renderer* renderer = GetSubsystem<Renderer>();

	VariantVector vpList = GetGlobalVar("ViewportVector").GetVariantVector();
	int numVP = vpList.Size();

	if (vId >= numVP)
	{
		URHO3D_LOGERROR("No viewport with that index.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//Viewport* vp = (Viewport*)GetGlobalVar("activeViewport").GetPtr();
	Viewport* vp = (Viewport*)vpList[vId].GetPtr();

	if (!vp)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	
	RenderPath* renderPath = vp->GetRenderPath();
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	String newRenderPath = inSolveInstance[0].GetString();
	XMLFile* rFile = cache->GetResource<XMLFile>(newRenderPath);

	if (rFile)
	{

		bool res = renderPath->Append(rFile);

		String tag;
		XMLElement rt = rFile->GetRoot().GetChild("rendertarget");
		if(!rt.IsNull())
		{
			tag = rt.GetAttribute("tag");
			trackedItems.Push(Pair<int, String>(vId, tag));
		}
		else
		{
			XMLElement cmd = rFile->GetRoot().GetChild("command");
			tag = cmd.GetAttribute("tag");
			trackedItems.Push(Pair<int, String>(vId, tag));
		}

		if (res)
		{
			//set some parameters
			VariantVector params = inSolveInstance[2].GetVariantVector();
			VariantVector vals = inSolveInstance[3].GetVariantVector();
			int num = Min(vals.Size(), params.Size());
			for (int i = 0; i < num; i++)
			{
				renderPath->SetShaderParameter(params[i].GetString(), vals[i]);
			}

			vp->SetRenderPath(renderPath);
		}
		
		outSolveInstance[0] = tag;
		return;

	}
	else
	{
		outSolveInstance[0] = Variant();
		return;
	}

	//Vector2 mix(source, bloom);
	//renderPath->SetShaderParameter("BloomHDRThreshold", threshold);
	//renderPath->SetShaderParameter("BloomHDRMix", mix);


}