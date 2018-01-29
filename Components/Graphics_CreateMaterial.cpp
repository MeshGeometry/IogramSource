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


#include "Graphics_CreateMaterial.h"

#include <Urho3D/Resource/ResourceCache.h>
#include "IoGraph.h"

#include "Urho3D/Graphics/Technique.h"
#include "Urho3D/Graphics/Material.h"
#include "Urho3D/Graphics/Texture2D.h"
#include "Urho3D/Graphics/TextureCube.h"
#include "Urho3D/Graphics/Texture2DArray.h"
#include "Urho3D/Graphics/Texture3D.h"

using namespace Urho3D;

String Graphics_CreateMaterial::iconTexture = "Textures/Icons/Graphics_CreateMaterial.png";

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

Graphics_CreateMaterial::Graphics_CreateMaterial(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("CreateMaterial");
	SetFullName("Create Material");
	SetDescription("Create a material from parameters");

	AddInputSlot(
		"Technique",
		"Tec",
		"Rendering technique specification.",
		VAR_STRING,
		ITEM,
		"Techniques/NoTexture.xml"
	);

	AddInputSlot(
		"Parameters",
		"P",
		"Named pairs of shader parameters.",
		VAR_VARIANTMAP,
		LIST
	);

	AddInputSlot(
		"Textures",
		"T",
		"Named pairs of texture units.",
		VAR_VARIANTMAP,
		LIST
	);
	
	AddInputSlot(
		"Vertex Defines",
		"VS",
		"Vertex shader defines",
		VAR_STRING,
		LIST
	);

	AddInputSlot(
		"Pixel Defines",
		"PS",
		"Pixel shader defines",
		VAR_STRING,
		LIST
	);

	AddOutputSlot(
		"Material",
		"M",
		"Material",
		VAR_PTR,
		ITEM
	);

	AddOutputSlot(
		"Resource",
		"R",
		"Resource Path",
		VAR_STRING,
		ITEM
	);
}

void Graphics_CreateMaterial::PreLocalSolve()
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


void Graphics_CreateMaterial::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	String techPath = inSolveInstance[0].GetString();

	Technique* technique = cache->GetResource<Technique>(techPath);

	//create the material
	SharedPtr<Material> mat(new Material(GetContext()));

	//must have a technique
	if (!technique)
	{
		URHO3D_LOGERROR("Could not load technique.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//set the technique
	mat->SetNumTechniques(1);
	mat->SetTechnique(0, technique);

	//iterate over the parameters and assign
	VariantVector params = inSolveInstance[1].GetVariantVector();
	for (int i = 0; i < params.Size(); i++)
	{
		
		if (params[i].GetType() != VAR_VARIANTMAP)
			continue;
		
		VariantMap vm = params[i].GetVariantMap();
		auto keys = vm.Keys();
		if (keys.Contains("name") && keys.Contains("value"))
		{
			String name = vm["name"].GetString();
			Variant val = vm["value"];

			mat->SetShaderParameter(name, val);
		}
	}

	//iterate over the textures and assign
	params = inSolveInstance[2].GetVariantVector();
	for (int i = 0; i < params.Size(); i++)
	{
		if (params[i].GetType() != VAR_VARIANTMAP)
			continue;
		
		VariantMap vm = params[i].GetVariantMap();
		auto keys = vm.Keys();
		if (keys.Contains("name") && keys.Contains("value"))
		{
			String name = vm["name"].GetString();
			Variant val = vm["value"];

			TextureUnit unit = ParseTextureUnitName(name);
			if (unit != MAX_TEXTURE_UNITS)
			{
				Texture* tex;
				if (val.GetType() == VAR_STRING)
				{
					tex = GetSubsystem<ResourceCache>()->GetResource<Texture>(val.GetString());
				}
				else if (val.GetType() == VAR_PTR)
				{
					tex = (Texture*)val.GetPtr();
				}

				//assign
				if (tex)
				{
					mat->SetTexture(unit, tex);
				}
			}
		}
	}

	//iterate over vs defines
	params = inSolveInstance[3].GetVariantVector();
	for (int i = 0; i < params.Size(); i++)
	{
		if (params[i].GetType() != VAR_STRING)
			continue;
		
		String define = params[i].GetString();
		if (!define.Empty())
		{
			mat->SetVertexShaderDefines(define);
		}
	}

	//iterate over ps defines
	params = inSolveInstance[4].GetVariantVector();
	for (int i = 0; i < params.Size(); i++)
	{
		if (params[i].GetType() != VAR_STRING)
			continue;
		
		String define = params[i].GetString();
		if (!define.Empty())
		{
			mat->SetPixelShaderDefines(define);
		}
	}

	//add to resource cache
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	Time* time = GetSubsystem<Time>();
	StringHash id(time->GetSystemTime());
	String name = "/tmp/materials/" + id.ToString();
	int counter = 0;
	while (rc->GetResource<Material>(name))
	{
		id = StringHash(time->GetSystemTime() + counter);
		name = "/tmp/materials/" + id.ToString();
		counter++;
	}
	mat->SetName("/tmp/materials/" + id.ToString());
	bool res = rc->AddManualResource(mat.Get());
	trackedItems.Push(mat->GetName());

	//debug
	//mat->SaveFile("mat_test.xml");

	outSolveInstance[0] = mat;
	outSolveInstance[1] = mat->GetName();
}

