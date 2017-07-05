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


#include "Graphics_Viewport.h"

#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/View3D.h>
#include <Urho3D/UI/UI.h>
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
		Vector4(10, 10, 200, 200)
		);

	AddInputSlot(
		"RenderPath",
		"RP",
		"Base RenderPath for viewport. If blank, this will default to the main viewport path.",
		VAR_STRING,
		DataAccess::ITEM,
		"RenderPaths/ForwardDepth.xml"
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
		UIElement* element = trackedItems[i];
		element->Remove();
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
	Camera* cam = (Camera*)inSolveInstance[0].GetPtr();
	if (!cam || !scene) {
		URHO3D_LOGERROR("A valid camera and scene is required for viewport creation.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	Vector4 vExtents = inSolveInstance[1].GetVector4();
	String renderPath = inSolveInstance[2].GetString();

	UIElement* activeRegion = (UIElement*)GetGlobalVar("activeUIRegion").GetPtr();

	if (!activeRegion) {
		activeRegion = GetSubsystem<UI>()->GetRoot();
	}

	View3D* view = activeRegion->CreateChild<View3D>("View3D");
	view->SetPosition(vExtents.x_, vExtents.y_);
	view->SetSize(vExtents.z_, vExtents.w_);
	view->SetView(scene, cam, false);

	//load renderpath
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	RenderPath* render_path = new RenderPath();
	render_path->Load(cache->GetResource<XMLFile>(renderPath));
	view->GetViewport()->SetRenderPath(render_path);
	view->SetAutoUpdate(true);

	trackedItems.Push(view);
	
	outSolveInstance[0] = view;

}