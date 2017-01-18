#pragma once

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>

class IogramPlayer : public Urho3D::Application {
	URHO3D_OBJECT(IogramPlayer, Urho3D::Application)
public:
	IogramPlayer(Urho3D::Context* context);
	virtual void Setup();
	virtual void Start();
	virtual void Stop();
	void CreateScene();
	void SetupViewport();
	void LoadPlugins();
	void SetUIScale();

public:
	//the scene
	static IogramPlayer* instance_; //singleton to app instance
	Urho3D::Scene* scene_;
	Urho3D::SharedPtr<Urho3D::Node> cameraNode_;
	Urho3D::SharedPtr<Urho3D::Node> lightNode_;

private:
	void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};