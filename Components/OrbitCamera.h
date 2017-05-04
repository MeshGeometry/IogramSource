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
// All Urho3D classes reside in namespace Urho3D
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/Camera.h>

enum CameraViews
{
	TOP,
	LEFT,
	RIGHT,
	BOTTOM,
	FRONT,
	BACK
};

/// Custom logic component for rotating a scene node.
class OrbitCamera : public Urho3D::LogicComponent
{
    URHO3D_OBJECT(OrbitCamera, Urho3D::LogicComponent)
    
public:
    /// Construct.
    OrbitCamera(Urho3D::Context* context);

    /// Handle scene update. Called by LogicComponent base class.
    virtual void Update(float timeStep);
    virtual void Start();
	Urho3D::Camera* camera_;

	///manipulations
	void SetView(CameraViews view);
	void ZoomExtents(Urho3D::Vector3 min, Urho3D::Vector3 max);
	void ZoomExtents();
private:
	Urho3D::Vector3 view_center_;
	Urho3D::SharedPtr<Urho3D::Node> cam_node_;
	Urho3D::SharedPtr<Urho3D::Node> light_node_;
	Urho3D::SharedPtr<Urho3D::Node> second_light_;
};