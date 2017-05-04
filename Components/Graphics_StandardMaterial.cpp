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


#include "Graphics_StandardMaterial.h"

#include <Urho3D/Resource/ResourceCache.h>
#include "IoGraph.h"

#include "Urho3D/Graphics/Technique.h"
#include "Urho3D/Graphics/Material.h"
#include "Urho3D/Graphics/Texture2D.h"
#include "Urho3D/Graphics/TextureCube.h"
#include "Urho3D/Graphics/Texture2DArray.h"
#include "Urho3D/Graphics/Texture3D.h"

using namespace Urho3D;

String Graphics_StandardMaterial::iconTexture = "Textures/Icons/Graphics_StandardMaterial.png";

namespace
{
	static const char* textureUnitNames[] =
	{
		"diffuse",
		"normal",
		"specular",
		"emissive",
		"environment",
#ifdef DESKTOP_GRAPHICS
		"volume",
		"custom1",
		"custom2",
		"lightramp",
		"lightshape",
		"shadowmap",
		"faceselect",
		"indirection",
		"depth",
		"light",
		"zone",
		0
#else
		"lightramp",
		"lightshape",
		"shadowmap",
		0
#endif
	};

	const char* cullModeNames[] =
	{
		"none",
		"ccw",
		"cw",
		0
	};

	static const char* fillModeNames[] =
	{
		"solid",
		"wireframe",
		"point",
		0
	};

	TextureUnit ParseTextureUnitName(String name)
	{
		name = name.ToLower().Trimmed();

		TextureUnit unit = (TextureUnit)GetStringListIndex(name.CString(), textureUnitNames, MAX_TEXTURE_UNITS);
		if (unit == MAX_TEXTURE_UNITS)
		{
			// Check also for shorthand names
			if (name == "diff")
				unit = TU_DIFFUSE;
			else if (name == "albedo")
				unit = TU_DIFFUSE;
			else if (name == "norm")
				unit = TU_NORMAL;
			else if (name == "spec")
				unit = TU_SPECULAR;
			else if (name == "env")
				unit = TU_ENVIRONMENT;
			// Finally check for specifying the texture unit directly as a number
			else if (name.Length() < 3)
				unit = (TextureUnit)Clamp(ToInt(name), 0, MAX_TEXTURE_UNITS - 1);
		}

		if (unit == MAX_TEXTURE_UNITS)
			URHO3D_LOGERROR("Unknown texture unit name " + name);

		return unit;
	}

	StringHash ParseTextureTypeName(String name)
	{
		name = name.ToLower().Trimmed();

		if (name == "texture")
			return Texture2D::GetTypeStatic();
		else if (name == "cubemap")
			return TextureCube::GetTypeStatic();
		else if (name == "texture3d")
			return Texture3D::GetTypeStatic();
		else if (name == "texturearray")
			return Texture2DArray::GetTypeStatic();

		return 0;
	}
}

Graphics_StandardMaterial::Graphics_StandardMaterial(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("CreateMaterial");
	SetFullName("Create Material");
	SetDescription("Create a material from parameters");


	AddInputSlot(
		"Diffuse",
		"D",
		"Diffuse Color",
		VAR_COLOR,
		ITEM,
		Color::CYAN
		);

	AddInputSlot(
		"Specular",
		"S",
		"Specular Color",
		VAR_COLOR,
		ITEM,
		Color::WHITE
		);

	AddInputSlot(
		"Emissive",
		"E",
		"Emissive Color",
		VAR_COLOR,
		ITEM,
		Color::BLACK
		);

	AddInputSlot(
		"Roughness",
		"R",
		"Roughness",
		VAR_FLOAT,
		ITEM,
		0.3f
		);

	AddInputSlot(
		"Metallic",
		"M",
		"Metallic",
		VAR_FLOAT,
		ITEM,
		0.4f
		);

	AddInputSlot(
		"Backface",
		"BF",
		"Backface",
		VAR_BOOL,
		ITEM,
		false
		);

	AddOutputSlot(
		"Resource",
		"R",
		"Resource Path",
		VAR_STRING,
		ITEM
		);
}

void Graphics_StandardMaterial::PreLocalSolve()
{
	//release resource
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	for (int i = 0; i < trackedItems.Size(); i++)
	{
		String resourcePath = trackedItems[i];
		if (resourcePath.Contains("tmp/materials/"))
			rc->ReleaseResource<Material>(trackedItems[i]);
	}
}


void Graphics_StandardMaterial::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	String techPath = inSolveInstance[0].GetString();

	//load the basic PBR technique
	Technique* technique = cache->GetResource<Technique>("Techniques/PBRNoTexture.xml");

	//create the material
	Material* sourceMat = cache->GetResource<Material>("Materials/BasicPBR.xml");
	SharedPtr<Material> mat = sourceMat->Clone();

	//must have a technique
	if (!technique)
	{
		URHO3D_LOGERROR("Could not load technique.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//get the input data
	Color diffColor = inSolveInstance[0].GetColor();
	Color specColor = inSolveInstance[1].GetColor();
	Color emissiveColor = inSolveInstance[2].GetColor();
	float roughness = inSolveInstance[3].GetFloat();
	roughness = Clamp(roughness, 0.0f, 1.0f);
	float metallic = inSolveInstance[4].GetFloat();
	metallic = Clamp(metallic, 0.0f, 1.0f);
	bool backface = inSolveInstance[5].GetBool();

	//check if diffColor has alpha < 1.0. If so, switch techniques
	if (diffColor.a_ < 1.0f) {
		technique = cache->GetResource<Technique>("Techniques/PBRNoTextureAlpha.xml");
		//fall back 
		if (!technique) {
			technique = cache->GetResource<Technique>("Techniques/PBRNoTexture.xml");
		}

		mat->SetNumTechniques(1);
		mat->SetTechnique(0, technique);
	}

	//set the technique
	//mat->SetNumTechniques(1);
	//mat->SetTechnique(0, technique);

	//set the params
	mat->SetShaderParameter("MatDiffColor", diffColor);
	mat->SetShaderParameter("MatSpecColor", specColor);
	mat->SetShaderParameter("MatEmissiveColor", emissiveColor);
	mat->SetShaderParameter("MatEnvMapColor", emissiveColor);
	mat->SetShaderParameter("Roughness", roughness);
	mat->SetShaderParameter("Metallic", metallic);
	mat->SetRenderOrder(128);
	mat->SetShadowCullMode(CULL_CCW);
	mat->SetCullMode(CULL_CCW);
	if (backface) {
		mat->SetCullMode(CULL_NONE);
	}
	mat->SetFillMode(FillMode::FILL_SOLID);

	//add to resource cache
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	Time* time = GetSubsystem<Time>();
	StringHash id(time->GetSystemTime());
	mat->SetName("/tmp/materials/" + id.ToString());
	bool res = rc->AddManualResource(mat.Get());
	trackedItems.Push(mat->GetName());

	//output resource path
	outSolveInstance[0] = mat->GetName();
}

