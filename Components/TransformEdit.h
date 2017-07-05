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

#include <Urho3D/Scene/Component.h>
#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/StaticModel.h>

/// Custom logic component for rotating a scene node.
class TransformEdit : public Urho3D::Component
{
	URHO3D_OBJECT(TransformEdit, Urho3D::Component)

public:
	/// Construct.
	TransformEdit(Urho3D::Context* context);
	~TransformEdit() {};

protected:

	bool editing_;
	Urho3D::String currentEditName_;

	Urho3D::Matrix3x4 lastTransform_;
	Urho3D::RayQueryResult raycastResult_;
	Urho3D::IntVector2 startScreenPos_;
	Urho3D::HashMap<Urho3D::StaticModel*, Urho3D::String> modelMap_;


protected:

	virtual void OnNodeSet(Urho3D::Node* node);
	
	void LoadPrimitive(Urho3D::String name);
	Urho3D::Ray GetScreenRay(Urho3D::IntVector2 screenPos);
	Urho3D::IntVector2 GetScaledMousePosition();
	void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleMouseDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleMouseUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleComponentRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	bool Raycast();

};