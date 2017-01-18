#pragma once

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>

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
}
