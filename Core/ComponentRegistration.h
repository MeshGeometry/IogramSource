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


#pragma once

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/IO/Log.h>

//static char* GRAPH_CATEGORY = "GRAPH_CATEGORY";
//static char* GRAPH_TYPES = "GRAPH_TYPES";
//static char* GRAPH_TAGS = "GRAPH_TAGS";

template <class T> static void RegisterIogramType(Urho3D::Context* context)
{
	//register with context
	context->RegisterFactory<T>("GRAPH_CATEGORY");

	//get or create the type map
	Urho3D::Variant typeVar = context->GetGlobalVar("GRAPH_TYPES");
	Urho3D::VariantMap typeMap = typeVar.GetVariantMap();

	//create a new info map for this type
	Urho3D::VariantMap infoMap;
	infoMap["typeName"] = T::GetTypeNameStatic();
	infoMap["texturePath"] = T::iconTexture;
	infoMap["isScript"] = false;

	//bunch of nasty hacks for tags
	Urho3D::String tagString = T::GetTypeNameStatic().Split('_')[0];
	tagString.Replace(',', ' '); //get rid of commas
	Urho3D::Vector<Urho3D::String> parts = tagString.Split(' ');//split in to parts
	Urho3D::VariantVector globalTags = context->GetGlobalVar("GRAPH_TAGS").GetVariantVector();
	Urho3D::String combinedString = "";
	for (unsigned i = 0; i < parts.Size(); i++)
	{
		Urho3D::String tag = parts[i].Trimmed();
		tag = tag.ToLower();

		if (!globalTags.Contains(tag))
		{
			globalTags.Push(tag);
		}

		combinedString += tag + " ";
	}

	//set the tag map
	infoMap["tags"] = combinedString;
	context->SetGlobalVar("GRAPH_TAGS", globalTags);

	//set the type map
	typeMap[T::GetTypeNameStatic()] = infoMap;
	context->SetGlobalVar("GRAPH_TYPES", typeMap);

	//log
	Urho3D::String msg;
	msg.AppendWithFormat("Registered type: %s", T::GetTypeNameStatic().CString());
	URHO3D_LOGINFO(msg);
}

static void RegisterIogramScript(Urho3D::Context* context, Urho3D::String path, Urho3D::String className)
{
	
	//get or create the type map
	Urho3D::Variant typeVar = context->GetGlobalVar("GRAPH_TYPES");
	Urho3D::VariantMap typeMap = typeVar.GetVariantMap();
	
	//create a new info map for this type
	Urho3D::VariantMap infoMap;
	infoMap["scriptPath"] = path;
	infoMap["typeName"] = "IoScriptInstance";
	infoMap["className"] = className;
	infoMap["texturePath"] = "";
	infoMap["tags"] = "Script";
	infoMap["isScript"] = true;

	typeMap[path] = infoMap;
	context->SetGlobalVar("GRAPH_TYPES", typeMap);
}
