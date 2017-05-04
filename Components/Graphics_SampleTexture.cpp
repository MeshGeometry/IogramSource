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


#include "Graphics_SampleTexture.h"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Core/Timer.h>

using namespace Urho3D;

String Graphics_SampleTexture::iconTexture = "Textures/Icons/Graphics_SampleTexture.png";

Graphics_SampleTexture::Graphics_SampleTexture(Context* context) : IoComponentBase(context, 0, 0)
{
	//set up component info
	SetName("RenderTexture");
	SetFullName("Render Camera to Texture");
	SetDescription("Creates a texture that is filled by the given camera.");

	AddInputSlot(
		"Image",
		"I",
		"Image or texture to sample",
		VAR_STRING,
		DataAccess::ITEM
		);

	AddInputSlot(
		"X",
		"X",
		"Pixel location in X direction",
		VAR_INT,
		DataAccess::ITEM,
		0
		);

	AddInputSlot(
		"Y",
		"Y",
		"Pixel location in Y direction",
		VAR_INT,
		DataAccess::ITEM,
		0
		);

	AddInputSlot(
		"Z",
		"Z",
		"Pixel location in Z direction",
		VAR_INT,
		DataAccess::ITEM,
		0
		);


	AddOutputSlot(
		"Color",
		"C",
		"Color",
		VAR_COLOR,
		DataAccess::ITEM
		);

	AddOutputSlot(
		"Greyscale",
		"BW",
		"Greyscale value",
		VAR_COLOR,
		DataAccess::ITEM
		);

	AddOutputSlot(
		"Luminance",
		"L",
		"Luminance value",
		VAR_COLOR,
		DataAccess::ITEM
		);
}


//work function
void Graphics_SampleTexture::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	)
{
	String imagePath = inSolveInstance[0].GetString();
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	
	Image* image = rc->GetResource<Image>(imagePath);
	if (!image)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}


	
	int dx = Abs(inSolveInstance[1].GetInt());
	int dy = Abs(inSolveInstance[2].GetInt());
	int dz = Abs(inSolveInstance[3].GetInt());

	//sample
	Color c = image->GetPixel(Min(dx, image->GetWidth()), Min(dy, image->GetHeight()), Min(dz, image->GetDepth()));

	float saturation = c.SaturationHSL();
	float luma = c.Luma();

	outSolveInstance[0] = c;
	outSolveInstance[1] = saturation;
	outSolveInstance[2] = luma;


}

