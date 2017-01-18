#include "Graphics_Viewport.h"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>

using namespace Urho3D;

String Graphics_Viewport::iconTexture = "Textures/Icons/Graphics_Viewport.png";

Graphics_Viewport::Graphics_Viewport(Context* context) : IoComponentBase(context, 0, 0)
{
	//set up component info
	SetName("Viewport");
	SetFullName("Create Viewport");
	SetDescription("Creates a viewport for viewing geometry");

	//set up the slots
	AddInputSlot(
		"Viewport Index",
		"ID",
		"Index of viewport to create. If exists, viewport will be overwritten.",
		VAR_INT,
		DataAccess::ITEM,
		1
		);

	AddInputSlot(
		"Camera",
		"C",
		"Camera with which to render viewport.",
		VAR_PTR,
		DataAccess::ITEM
		);

	AddInputSlot(
		"Rect",
		"R",
		"Screen space rectangle in which to render. If null, full extents are used.",
		VAR_VECTOR4,
		DataAccess::ITEM,
		Vector4(100, 100, 500, 500)
		);

	AddInputSlot(
		"RenderPath",
		"RP",
		"Base RenderPath for viewport. If blank, this will default to the main viewport path.",
		VAR_STRING,
		DataAccess::ITEM,
		"RenderPaths/PBRDeferred.xml"
		);

	AddOutputSlot(
		"Viewport",
		"VP",
		"Point to resulting viewport",
		VAR_PTR,
		DataAccess::ITEM
		);
}

void Graphics_Viewport::PreLocalSolve()
{
	//not sure how to release a viewport...
	for (int i = 0; i < trackedItems.Size(); i++)
	{
		int id = trackedItems[i];
		GetSubsystem<Renderer>()->SetViewport(id, NULL);
	}
	
	trackedItems.Clear();
}

//work function
void Graphics_Viewport::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	)
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	
	//first check if a camera has been set
	Camera* cam = (Camera*)inSolveInstance[1].GetPtr();
	if (!cam || !scene) {
		URHO3D_LOGERROR("A valid camera and scene is required for viewport creation.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	int viewID = inSolveInstance[0].GetInt();
	Vector4 vExtents = inSolveInstance[2].GetVector4();
	String renderPath = inSolveInstance[3].GetString();

	SharedPtr<Viewport> vp(new Viewport(GetContext(), scene, cam));
	IntRect viewRect(vExtents.x_, vExtents.y_, vExtents.z_, vExtents.w_);
	vp->SetRect(viewRect);
	
	GetSubsystem<Renderer>()->SetViewport(viewID, vp);

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	RenderPath* render_path = new RenderPath();

#ifdef EMSCRIPTEN
	render_path->Load(cache->GetResource<XMLFile>("RenderPaths/ForwardDepthSAO.xml"));
#endif

#ifndef EMSCRIPTEN
	render_path->Load(cache->GetResource<XMLFile>(renderPath));
#endif

	vp->SetRenderPath(render_path);

	trackedItems.Push(viewID);
	
	outSolveInstance[0] = vp;

}