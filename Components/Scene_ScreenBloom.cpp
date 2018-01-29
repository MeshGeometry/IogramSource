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


#include "Scene_ScreenBloom.h"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>

using namespace Urho3D;

String Scene_ScreenBloom::iconTexture = "Textures/ScreenBloom.png";

Scene_ScreenBloom::Scene_ScreenBloom(Urho3D::Context* context) : IoComponentBase(context, 4, 1)
{
	SetName("ScreenBloom");
	SetFullName("Screen Bloom");
	SetDescription("Adds intensity-controlled screen bloom effect");
	SetGroup(IoComponentGroup::DISPLAY);
	SetSubgroup("");

	inputSlots_[0]->SetName("Intensity");
	inputSlots_[0]->SetVariableName("I");
	inputSlots_[0]->SetDescription("Intensity of Bloom");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(75.0f);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Source");
	inputSlots_[1]->SetVariableName("S");
	inputSlots_[1]->SetDescription("Source Mix");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(90.0f);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Bloom Mix");
	inputSlots_[2]->SetVariableName("B");
	inputSlots_[2]->SetDescription("Bloom Mix");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(90.0f);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("Viewport ID");
	inputSlots_[3]->SetVariableName("V");
	inputSlots_[3]->SetDescription("Viewport ID");
	inputSlots_[3]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(0);
	inputSlots_[3]->DefaultSet();

	outputSlots_[0]->SetName("Vertices");
	outputSlots_[0]->SetVariableName("V");
	outputSlots_[0]->SetDescription("Vertices Out");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

}

void Scene_ScreenBloom::PreLocalSolve()
{
	//Renderer* renderer(GetSubsystem<Renderer>());
	//
	//RenderPath* renderPath;
	//if (renderer->GetViewport(0))
	//	renderPath = renderer->GetViewport(0)->GetRenderPath();
	//else
	//	return;

	//renderPath->RemoveCommands("BloomHDR");
	//renderPath->RemoveRenderTargets("BloomHDR");

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
				RenderPath* renderPath = vp->GetRenderPath();
				renderPath->RemoveCommands(rpCommand);
				renderPath->RemoveRenderTargets(rpCommand);
			}

		}

	}

	trackedItems.Clear();

}

void Scene_ScreenBloom::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	float threshold = inSolveInstance[0].GetFloat()/100.0f;
	float source = inSolveInstance[1].GetFloat() / 100.0f;
	float bloom = inSolveInstance[2].GetFloat() / 100.0f;
	int vId = inSolveInstance[3].GetInt(); 

	Renderer* renderer(GetSubsystem<Renderer>());

	VariantVector vpList = GetGlobalVar("ViewportVector").GetVariantVector();
	int numVP = vpList.Size();
	if (vId >= numVP || vId < 0)
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
	XMLFile* rFile = cache->GetResource<XMLFile>("PostProcess/BloomHDR.xml");

	if (rFile)
	{

		bool res = renderPath->Append(rFile);

		String tag;
		XMLElement rt = rFile->GetRoot().GetChild("rendertarget");
		if (!rt.IsNull())
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
			Vector2 mix(source, bloom);
			renderPath->SetShaderParameter("BloomHDRThreshold", threshold);
			renderPath->SetShaderParameter("BloomHDRMix", mix);

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




	//////


	//RenderPath* renderPath;
	//if (renderer->GetViewport(vId))
	//	renderPath = renderer->GetViewport(vId)->GetRenderPath();
	//else {
	//	outSolveInstance[0] = Variant();
	//	return;
	//}
	//
	//ResourceCache* cache = GetSubsystem<ResourceCache>();

	//renderPath->Append(cache->GetResource<XMLFile>("PostProcess/BloomHDR.xml"));

	//Vector2 mix(source, bloom);
	//renderPath->SetShaderParameter("BloomHDRThreshold", threshold);
	//renderPath->SetShaderParameter("BloomHDRMix", mix);
}