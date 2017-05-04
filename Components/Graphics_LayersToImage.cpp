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


#include "Graphics_LayersToImage.h"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Texture3D.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Core/Timer.h>

using namespace Urho3D;

String Graphics_LayersToImage::iconTexture = "Textures/Icons/Graphics_LayersToImage.png";

Graphics_LayersToImage::Graphics_LayersToImage(Context* context) : IoComponentBase(context, 0, 0)
{
	//set up component info
	SetName("RenderTexture");
	SetFullName("Render Camera to Texture");
	SetDescription("Creates a texture that is filled by the given camera.");

	AddInputSlot(
		"Layers",
		"L",
		"Layers to create image from",
		VAR_STRING,
		DataAccess::LIST
	);


	AddOutputSlot(
		"Texture",
		"T",
		"Texture",
		VAR_PTR,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Image",
		"I",
		"Image path",
		VAR_STRING,
		DataAccess::ITEM
	);

}

void Graphics_LayersToImage::PreLocalSolve()
{
	//release resource
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	for (int i = 0; i < trackedItems.Size(); i++)
	{
		String resourcePath = trackedItems[i];
		if (resourcePath.Contains("tmp/textures/"))
			rc->ReleaseResource<Texture2D>(trackedItems[i]);
		if (resourcePath.Contains("tmp/images/"))
			rc->ReleaseResource<Material>(trackedItems[i]);
	}
}

//work function
void Graphics_LayersToImage::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	
	ResourceCache* rc = GetSubsystem<ResourceCache>();	
	VariantVector texPaths = inSolveInstance[0].GetVariantVector();
	PODVector<Image*> texLayers;

	int dx = -1;
	int dy = -1;
	int dz = -1;

	if (texPaths.Empty() || texPaths[0].GetType() == VAR_NONE)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	for (int i = 0; i < texPaths.Size(); i++)
	{
		if (texPaths[i].GetType() == VAR_STRING)
		{
			String path = texPaths[i].GetString();
			Image* t = rc->GetResource<Image>(path);
			if (t)
			{
					texLayers.Push(t);
			}
		}
		else if (texPaths[i].GetType() == VAR_PTR)
		{
			Image* t = (Image*)texPaths[i].GetPtr();
			if (t)
			{
				texLayers.Push(t);
			}
		}

		//assign target dims from first tex
		if (texLayers.Size() == 1)
		{
			dx = texLayers[0]->GetWidth();
			dy = texLayers[0]->GetHeight();
		}
		else if(texLayers.Size() > 1) //check that we have constant dims
		{
			int cDX = texLayers[texLayers.Size() - 1]->GetWidth();
			int cDY = texLayers[texLayers.Size() - 1]->GetHeight();

			if (cDX != dx || cDY != dy)
			{
				SetAllOutputsNull(outSolveInstance);
				URHO3D_LOGERROR("LayersToImage: all image layers must be the same size.");
				return;
			}
		}

	}

	if (texLayers.Empty() || texPaths[0].GetType() == VAR_NONE)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}


	//assign z resoluion
	dz = texLayers.Size();

	//set image data
	SharedPtr<Image> tmpImage(new Image(GetContext()));
	tmpImage->SetSize(dx, dy, dz, 4);
	for (int i = 0; i < dz; i++)
	{
		Image* currLayer = texLayers[i];
		
		for (int j = 0; j < dy; j++)
		{
			for (int k = 0; k < dx; k++)
			{
				Color c = currLayer->GetPixel(k, j);
				tmpImage->SetPixel(k, j, i, c);
			}
		}
	}

	//create the texture
	SharedPtr<Texture3D> texOut(new Texture3D(GetContext()));
	texOut->SetSize(dx, dy, dz, Graphics::GetRGBAFormat(), TEXTURE_STATIC);
	texOut->SetData(tmpImage.Get(), true);

	//push to resource cache
	Time* time = GetSubsystem<Time>();
	StringHash id(time->GetSystemTime());
	texOut->SetName("/tmp/textures/" + id.ToString());
	bool res = rc->AddManualResource(texOut.Get());
	trackedItems.Push(texOut->GetName());

	//also add the image
	tmpImage->SetName("/tmp/images/" + id.ToString());
	rc->AddManualResource(tmpImage);
	trackedItems.Push(tmpImage->GetName());


	outSolveInstance[0] = texOut;
	outSolveInstance[1] = tmpImage->GetName();
}

