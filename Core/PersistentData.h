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