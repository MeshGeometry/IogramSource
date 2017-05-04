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
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/TextureCube.h>



/// Custom logic component for rotating a scene node.
class ReflectionProbe : public Urho3D::Component
{
	URHO3D_OBJECT(ReflectionProbe, Urho3D::Component)

public:
	/// Construct.
	ReflectionProbe(Urho3D::Context* context);
	~ReflectionProbe(){};

	bool Initialize();
	void UpdateReflectionMap();
	void SetResolution(int res) { resolution_ = res; };
	int GetResolution() { return resolution_; };

private:

	int initialized_;
	int resolution_;

	Urho3D::Viewport* viewportArray_[6];
	Urho3D::Camera* cameraArray_[6];
	Urho3D::TextureCube* reflectionMap_;

	void HandleRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

};