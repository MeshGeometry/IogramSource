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

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Viewport.h>

class IogramPlayer : public Urho3D::Application {
	URHO3D_OBJECT(IogramPlayer, Urho3D::Application)
public:
	IogramPlayer(Urho3D::Context* context);
	virtual void Setup();
	virtual void Start();
	virtual void Stop();
	void CreateScene();
	void SetupViewport();
	void LoadGraph();
	void LoadPlugins();
	void SetUIScale();

public:
	//the scene
	static IogramPlayer* instance_; //singleton to app instance
	Urho3D::Scene* scene_;
	Urho3D::Viewport* viewport_;
	Urho3D::Node* cameraNode_;
	Urho3D::Node* lightNode_;

private:
	void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};