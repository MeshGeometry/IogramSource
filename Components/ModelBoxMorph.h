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
class ModelBoxMorph : public Urho3D::Component
{
	URHO3D_OBJECT(ModelBoxMorph, Urho3D::Component)

public:
	/// Construct.
	ModelBoxMorph(Urho3D::Context* context);
	~ModelBoxMorph() {};

	void SetBaseGeometry(const Urho3D::Variant geometry);
	void GetCurrentGeometry(Urho3D::Variant& geometryOut);
	void SetEditing(bool editOn) { editing_ = editOn; }
	void DoHarmonicDeformation(Urho3D::Vector<Urho3D::Vector3> deltas, Urho3D::Vector<int> ids, Urho3D::Variant& geomOut);
	void DoLinearDeformation(Urho3D::Vector<Urho3D::Vector3> deltas, Urho3D::Vector<int> ids, Urho3D::Variant& geomOut);
    void DoBoxMorph(Urho3D::Vector<Urho3D::Vector3>& morphed_verts, Urho3D::Variant& geomOut);
	void UpdateHandles();

protected:

	bool editing_;
	Urho3D::RayQueryResult raycastResult_;
	Urho3D::IntVector2 startScreenPos_;
	Urho3D::BillboardSet* meshEditor_;
	Urho3D::Variant baseGeometry_;
	Urho3D::Variant boundingBox_;
	Urho3D::Vector<Urho3D::Vector3> originalVertParams_;
	Urho3D::Vector3 primaryDelta_;
	int primaryVertexID_;

protected:

	virtual void OnNodeSet(Urho3D::Node* node);
	void CreateMeshEditor();
	bool GetScreenRay(Urho3D::IntVector2 screenPos, Urho3D::Ray& ray);
	Urho3D::IntVector2 GetScaledMousePosition();
	void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleMouseDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleMouseUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleComponentRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	bool Raycast();

};
