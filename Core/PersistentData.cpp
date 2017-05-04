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


#include "PersistentData.h"

#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

PersistentData::PersistentData(Context* context) :Object(context)
{

}

bool PersistentData::RegisterAppData(String groupName, String appName)
{
	dataPath_ = GetSubsystem<FileSystem>()->GetAppPreferencesDir(groupName, appName) + appName + "Data.json";

	//load in to temp storage
	Load();

	//save
	Save();

	return true;
}

bool PersistentData::Save()
{
	//create the file
	File* dest = new File(context_, dataPath_, Urho3D::FileMode::FILE_WRITE);

	if (!dest || dest->GetMode() != FileMode::FILE_WRITE) {
		dest->Close();
		return false;
	}
		

	if (tempStorage_.Size() == 0) {
		dest->Close();
		return false;
	}

	//create the json file
	SharedPtr<JSONFile> json(new JSONFile(context_));
	JSONValue& rootElem = json->GetRoot();

	HashMap<String, Pair<String, Variant>>::ConstIterator itr;
	for (itr = tempStorage_.Begin(); itr != tempStorage_.End(); itr++)
	{
		const String keyName = itr->second_.first_;
		const Variant var = itr->second_.second_;

		if (keyName == "")
			continue;

		JSONValue varVal;
		varVal.Set("type", var.GetTypeName());
		varVal.Set("value", var.ToString());
		rootElem.Set(keyName, varVal);
	}

	//write it to disk	
	bool res = json->Save(*dest);
	dest->Close();
	return res;
}

bool PersistentData::Load()
{
	//try to load
	File* dest = new File(context_, dataPath_, Urho3D::FileMode::FILE_READ);

	if (!dest || dest->GetMode() != FileMode::FILE_READ)
		return false;

	//create the json file
	SharedPtr<JSONFile> json(new JSONFile(context_));
	json->Load(*dest);
	
	const JSONValue& rootElem = json->GetRoot();

	ConstJSONObjectIterator itr;
	for (itr = rootElem.Begin(); itr != rootElem.End(); itr++)
	{
		String keyName = itr->first_;
		const JSONValue& jVal = itr->second_;

		String type = jVal["type"].GetString();
		String value = jVal["value"].GetString();
		Variant val(type, value);

		Pair<String, Variant> p(keyName, val);
		tempStorage_[keyName] = p;
	}

	dest->Close();

	return false;
}

bool PersistentData::SetGenericData(String key, Variant data)
{
	if (data.GetType() == VAR_VARIANTVECTOR 
		|| data.GetType() == VAR_VARIANTMAP
		|| data.GetType() == VAR_VOIDPTR
		|| data.GetType() == VAR_PTR)
	{
		URHO3D_LOGERROR("Persistent Data doesn't support this type: " + data.GetTypeName());
		return false;
	}
	
	Pair<String, Variant> newPair(key, data);
	tempStorage_[key] = newPair;

	Save();

	return true;
}

Variant PersistentData::GetGenericData(String key)
{	
	Pair<String, Variant> p = tempStorage_[key];
	return p.second_;
}

String PersistentData::GetString(String key)
{
	Pair<String, Variant> p = tempStorage_[key];
	return p.second_.GetString();
}

bool PersistentData::GetBool(String key)
{
	Pair<String, Variant> p = tempStorage_[key];
	return p.second_.GetBool();
}

float PersistentData::GetFloat(String key)
{
	Pair<String, Variant> p = tempStorage_[key];
	return p.second_.GetFloat();
}

Vector3 PersistentData::GetVector3(String key)
{
	Pair<String, Variant> p = tempStorage_[key];
	return p.second_.GetVector3();
}
