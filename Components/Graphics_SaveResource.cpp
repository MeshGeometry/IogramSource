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


#include "Graphics_SaveResource.h"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/Resource.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/Container/Str.h>

using namespace Urho3D;

String Graphics_SaveResource::iconTexture = "Textures/Icons/Graphics_SaveResource.png";

Graphics_SaveResource::Graphics_SaveResource(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("SaveResource");
	SetFullName("Save Resource");
	SetDescription("Saves a resource to a path");

	AddInputSlot(
		"Resource",
		"R",
		"Resource to save.",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Directory",
		"D",
		"Directory Path save resource.",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Name",
		"N",
		"Name to save resource.",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Reference",
		"RF",
		"Reference",
		VAR_STRING,
		DataAccess::ITEM
	);
}

void Graphics_SaveResource::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	FileSystem* fs = GetSubsystem<FileSystem>();


	String resPath = inSolveInstance[0].GetString();
	Resource* resource = rc->GetResource<Resource>(resPath);

	if (!resource)
	{
		resource = (Resource*)inSolveInstance[0].GetPtr();
	}

	if (!resource)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}


	String dirPath = inSolveInstance[1].GetString();
	String name = inSolveInstance[2].GetString();

	if (!fs->DirExists(dirPath))
	{
		fs->CreateDir(dirPath);
	}

	
	dirPath = AddTrailingSlash(dirPath);

	bool res = resource->SaveFile(dirPath + name);

	outSolveInstance[0] = dirPath + name;

}
