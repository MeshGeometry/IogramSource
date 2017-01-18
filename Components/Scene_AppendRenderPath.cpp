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
	Renderer* renderer(GetSubsystem<Renderer>());
	RenderPath* renderPath = renderer->GetViewport(0)->GetRenderPath();

	for (int i = 0; i < trackedItems.Size(); i++)
	{
		renderPath->RemoveCommands(trackedItems[i]);
		renderPath->RemoveRenderTargets(trackedItems[i]);
	}

	trackedItems.Clear();
}

void Scene_AppendRenderPath::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	int vId = inSolveInstance[1].GetInt();
	
	Renderer* renderer(GetSubsystem<Renderer>());

	if (vId >= renderer->GetNumViewports())
	{
		URHO3D_LOGERROR("No viewport with that index.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	RenderPath* renderPath = renderer->GetViewport(vId)->GetRenderPath();
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
			trackedItems.Push(tag);
		}
		else
		{
			XMLElement cmd = rFile->GetRoot().GetChild("command");
			tag = cmd.GetAttribute("tag");
			trackedItems.Push(tag);
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