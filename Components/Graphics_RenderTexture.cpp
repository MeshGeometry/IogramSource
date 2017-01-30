#include "Graphics_RenderTexture.h"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Graphics/Technique.h>

using namespace Urho3D;

String Graphics_RenderTexture::iconTexture = "Textures/Icons/Graphics_RenderTexture.png";

Graphics_RenderTexture::Graphics_RenderTexture(Context* context) : IoComponentBase(context, 0, 0)
{
	//set up component info
	SetName("RenderTexture");
	SetFullName("Render Camera to Texture");
	SetDescription("Creates a texture that is filled by the given camera.");

	AddInputSlot(
		"Camera",
		"C",
		"Camera with which to render viewport.",
		VAR_PTR,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Viewport",
		"VP",
		"Optional viewport to use for render texture",
		VAR_PTR,
		DataAccess::ITEM
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
		"Texture",
		"T",
		"Render Texture",
		VAR_PTR,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Material",
		"M",
		"Basic Material with the RenderTexture assigned.",
		VAR_PTR,
		DataAccess::ITEM
	);

	SubscribeToEvent(E_RENDERSURFACEUPDATE, URHO3D_HANDLER(Graphics_RenderTexture, HandleRenderUpdate));
}

void Graphics_RenderTexture::PreLocalSolve()
{
	//release resource
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	for (int i = 0; i < trackedItems.Size(); i++)
	{
		String resourcePath = trackedItems[i];
		if (resourcePath.Contains("tmp/textures/"))
			rc->ReleaseResource<Texture2D>(trackedItems[i]);
		if (resourcePath.Contains("tmp/materials/"))
			rc->ReleaseResource<Material>(trackedItems[i]);
	}
}

//work function
void Graphics_RenderTexture::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();

	//first check if a camera has been set
	Camera* cam = (Camera*)inSolveInstance[0].GetPtr();
	if (!cam)
	{
		cam = (Camera*)inSolveInstance[0].GetVoidPtr();
	}
	if (!cam || !scene) {
		URHO3D_LOGERROR("A valid camera and scene is required for viewport creation.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Viewport* vp = (Viewport*)inSolveInstance[1].GetPtr();
	if (vp)
	{
		//handle this cases
	}

	ResourceCache* cache = GetSubsystem<ResourceCache>();

	// Create a renderable texture (1024x768, RGB format), enable bilinear filtering on it
	SharedPtr<Texture2D> renderTexture(new Texture2D(context_));
	renderTexture->SetSize(1024, 1024, Graphics::GetRGBFormat(), TEXTURE_RENDERTARGET);
	renderTexture->SetFilterMode(FILTER_BILINEAR);
	renderTexture->SetName("tmp/textures/render_texture_" + String(0));
	cache->AddManualResource(renderTexture);
	trackedItems.Push(renderTexture->GetName());

	// Create a new material from scratch, use the diffuse unlit technique, assign the render texture
	// as its diffuse texture, then assign the material to the screen plane object
	SharedPtr<Material> renderMaterial(new Material(context_));
	renderMaterial->SetTechnique(0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
	renderMaterial->SetTexture(TU_DIFFUSE, renderTexture);
	renderMaterial->SetName("tmp/materials/render_material_" + String(0));
	cache->AddManualResource(renderMaterial);
	trackedItems.Push(renderMaterial->GetName());

	// Since the screen material is on top of the box model and may Z-fight, use negative depth bias
	// to push it forward (particularly necessary on mobiles with possibly less Z resolution)
	renderMaterial->SetDepthBias(BiasParameters(-0.001f, 0.0f));

	// Get the texture's RenderSurface object (exists when the texture has been created in rendertarget mode)
	// and define the viewport for rendering the second scene, similarly as how backbuffer viewports are defined
	// to the Renderer subsystem. By default the texture viewport will be updated when the texture is visible
	// in the main view
	RenderSurface* surface = renderTexture->GetRenderSurface();
	SharedPtr<Viewport> rttViewport(new Viewport(context_, scene, cam));
	surface->SetViewport(0, rttViewport);

	//set outputs
	outSolveInstance[0] = renderTexture;
	outSolveInstance[1] = renderMaterial;

}

void Graphics_RenderTexture::HandleRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	for (int i = 0; i < trackedItems.Size(); i++)
	{
		String resourcePath = trackedItems[i];
		if (resourcePath.Contains("tmp/textures/"))
		{
			Texture2D* rt = rc->GetResource<Texture2D>(trackedItems[i]);
			if (rt)
			{
				RenderSurface* rf = rt->GetRenderSurface();
				if (rf)
				{
					rf->QueueUpdate();
				}
			}
		}

	}
}