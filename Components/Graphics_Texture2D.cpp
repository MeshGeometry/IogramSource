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


#include "Graphics_Texture2D.h"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Core/Timer.h>

using namespace Urho3D;

String Graphics_Texture2D::iconTexture = "Textures/Icons/Graphics_Texture2D.png";

Graphics_Texture2D::Graphics_Texture2D(Context* context) : IoComponentBase(context, 0, 0)
{
	//set up component info
	SetName("RenderTexture");
	SetFullName("Render Camera to Texture");
	SetDescription("Creates a texture that is filled by the given camera.");

	AddInputSlot(
		"DimX",
		"X",
		"Pixel dimensions in X direction",
		VAR_INT,
		DataAccess::ITEM,
		256
		);

	AddInputSlot(
		"DimY",
		"Y",
		"Pixel dimensions in Y direction",
		VAR_INT,
		DataAccess::ITEM,
		256
		);

	AddInputSlot(
		"Colors",
		"C",
		"Color array to fill texture",
		VAR_COLOR,
		DataAccess::LIST,
		Color::CYAN
		);

	AddInputSlot(
		"Name",
		"N",
		"Optional name for texture resource.",
		VAR_STRING,
		DataAccess::ITEM
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

void Graphics_Texture2D::PreLocalSolve()
{
	//release resource
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	for (int i = 0; i < trackedItems.Size(); i++)
	{
		String resourcePath = trackedItems[i];
		if (resourcePath.Contains("tmp/textures/"))
			rc->ReleaseResource<Texture2D>(trackedItems[i]);
		if (resourcePath.Contains("tmp/iamges/"))
			rc->ReleaseResource<Image>(trackedItems[i]);
	}
}

//work function
void Graphics_Texture2D::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	)
{
	int dx = inSolveInstance[0].GetInt();
	int dy = inSolveInstance[1].GetInt();

	dx = Clamp(dx, 1, 4096);
	dy = Clamp(dy, 1, 4096);

	VariantVector colors = inSolveInstance[2].GetVariantVector();
	int numColors = colors.Size();

	//set image data
	SharedPtr<Image> tmpImage(new Image(GetContext()));
	tmpImage->SetSize(dx, dy, 1, 4);
	for (int i = 0; i < dy; i++)
	{
		for (int j = 0; j < dx; j++)
		{
			Color c = colors[(i*dy + j) % numColors].GetColor();
			tmpImage->SetPixel(j, i, c);
		}
	}

	//create the texture
	SharedPtr<Texture2D> texOut(new Texture2D(GetContext()));
	texOut->SetSize(dx, dy, Graphics::GetRGBAFormat(), TEXTURE_STATIC);
	texOut->SetData(tmpImage.Get(), true);

	//push to resource cache
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	Time* time = GetSubsystem<Time>();
	StringHash id(time->GetSystemTime());
	texOut->SetName("/tmp/textures/" + id.ToString());
	bool res = rc->AddManualResource(texOut.Get());
	trackedItems.Push(texOut->GetName());

	//also add the image
	tmpImage->SetName("/tmp/images/" + id.ToString());
	rc->AddManualResource(tmpImage);
	trackedItems.Push(tmpImage->GetName());

	//debug
	//res = tmpImage->SavePNG(id.ToString() + ".png");

	outSolveInstance[0] = texOut;
	outSolveInstance[1] = tmpImage->GetName();
}

