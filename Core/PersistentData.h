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

#include <Urho3D/Core/Object.h>
#include <Urho3D/Resource/JSONFile.h>

class URHO3D_API PersistentData : public Urho3D::Object
{
	URHO3D_OBJECT(PersistentData, Urho3D::Object)
public:
	PersistentData(Urho3D::Context* context);
	~PersistentData(){};

	bool RegisterAppData(Urho3D::String groupName, Urho3D::String appName);

	bool SetGenericData(Urho3D::String key, Urho3D::Variant data);
	Urho3D::Variant GetGenericData(Urho3D::String key);
	Urho3D::String GetString(Urho3D::String key);
	bool GetBool(Urho3D::String key);
	float GetFloat(Urho3D::String key);
	Urho3D::Vector3 GetVector3(Urho3D::String key);

	bool Save();
	bool Load();
	
private:

	Urho3D::String dataPath_;
	Urho3D::HashMap<Urho3D::String, Urho3D::Pair<Urho3D::String, Urho3D::Variant>> tempStorage_;
};