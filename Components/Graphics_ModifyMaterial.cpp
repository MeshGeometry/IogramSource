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


#include "Graphics_ModifyMaterial.h"

#include <Urho3D/Resource/ResourceCache.h>
#include "IoGraph.h"

#include "Urho3D/Graphics/Technique.h"
#include "Urho3D/Graphics/Material.h"
#include "Urho3D/Graphics/Texture2D.h"
#include "Urho3D/Graphics/TextureCube.h"
#include "Urho3D/Graphics/Texture2DArray.h"
#include "Urho3D/Graphics/Texture3D.h"

using namespace Urho3D;

String Graphics_ModifyMaterial::iconTexture = "Textures/Icons/Graphics_ModifyMaterial.png";

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

Graphics_ModifyMaterial::Graphics_ModifyMaterial(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("CreateMaterial");
	SetFullName("Create Material");
	SetDescription("Create a material from parameters");

	AddInputSlot(
		"Material",
		"M",
		"Material to modify",
		VAR_STRING,
		ITEM
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
}


void Graphics_ModifyMaterial::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	String matPath = inSolveInstance[0].GetString();

	Material* mat = cache->GetResource<Material>(matPath);
	if (!mat && inSolveInstance[0].GetType() == VAR_PTR)
	{
		mat = (Material*)inSolveInstance[0].GetPtr();
	}

	if (!mat)
	{
		URHO3D_LOGERROR("Could not load material.");
		SetAllOutputsNull(outSolveInstance);
		return;

	}


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

	outSolveInstance[0] = mat;
}

