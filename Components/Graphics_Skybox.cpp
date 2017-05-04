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


#include "Graphics_Skybox.h"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/TextureCube.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Technique.h>

using namespace Urho3D;

String Graphics_Skybox::iconTexture = "Textures/Icons/Graphics_Skybox.png";

Graphics_Skybox::Graphics_Skybox(Context* context) : IoComponentBase(context, 0, 0)
{
	//set up component info
	SetName("RenderTexture");
	SetFullName("Render Camera to Texture");
	SetDescription("Creates a texture that is filled by the given camera.");

	AddInputSlot(
		"ColorA",
		"A",
		"First color for gradient",
		VAR_COLOR,
		DataAccess::ITEM,
		Color(0.8f, 0.8f, 0.8f, 1.f)
	);

	AddInputSlot(
		"ColorB",
		"B",
		"Second color for gradient",
		VAR_COLOR,
		DataAccess::ITEM,
		Color(0.2f, 0.2f, 0.3f, 1.f)
	);

	AddInputSlot(
		"BlurRadius",
		"BR",
		"Radius for blur.",
		VAR_FLOAT,
		DataAccess::ITEM,
		0.2f
	);

	AddInputSlot(
		"Blend",
		"BL",
		"Blend factor between texture and gradient.",
		VAR_FLOAT,
		DataAccess::ITEM,
		0.2f
	);

	AddInputSlot(
		"Texture",
		"T",
		"Optional texture.",
		VAR_STRING,
		DataAccess::ITEM,
		"Textures/studio008.hdr"
	);

	AddOutputSlot(
		"Skybox",
		"S",
		"Skybox reference.",
		VAR_PTR,
		DataAccess::ITEM
	);

}

void Graphics_Skybox::PreLocalSolve()
{
	
}

//work function
void Graphics_Skybox::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();

	if (!scene) {
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	Skybox* skybox = scene->GetComponent<Skybox>();
	if (!skybox) {
		skybox = scene->CreateComponent<Skybox>();
		skybox->SetModel(cache->GetResource<Model>("Models/Sphere.mdl"));
	}



	//get the data
	Color colA = inSolveInstance[0].GetColor();
	Color colB = inSolveInstance[1].GetColor();
	float blurRadius = inSolveInstance[2].GetFloat();
	float blend = inSolveInstance[3].GetFloat();
	String texPath = inSolveInstance[4].GetString();

	Texture2D* tex = cache->GetResource<Texture2D>(texPath);
	if (!tex) {
		tex = (Texture2D*)inSolveInstance[4].GetPtr();
	}

	//set texture parameters
	Material* mat = cache->GetResource<Material>("Materials/GradientSkydome.xml");
	if (tex) {
		mat->SetTexture(TU_DIFFUSE, tex);
	}
	mat->SetShaderParameter("GradColorA", colA);
	mat->SetShaderParameter("GradColorB", colB);
	mat->SetShaderParameter("BlurRadius", 0.001f * blurRadius);
	mat->SetShaderParameter("Blend", blend);
	skybox->SetMaterial(mat);

	outSolveInstance[0] = skybox;
}
