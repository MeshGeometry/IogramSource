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

#include "TransformEdit.h"

#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

TransformEdit::TransformEdit(Context* context) : Component(context),
	editing_(false)
{

}

void TransformEdit::OnNodeSet(Urho3D::Node* node)
{
	Component::OnNodeSet(node);

	if (!node)
	{
		return;
	}

	//need to get a bunch of assets
	LoadPrimitive("MoveX");
	LoadPrimitive("MoveY");
	LoadPrimitive("MoveZ");
	LoadPrimitive("RotateX");
	LoadPrimitive("RotateY");
	LoadPrimitive("RotateZ");

	//subscribe to events
	SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(TransformEdit, HandleMouseDown));
	SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(TransformEdit, HandleMouseMove));
	SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(TransformEdit, HandleMouseUp));
	SubscribeToEvent(E_COMPONENTREMOVED, URHO3D_HANDLER(TransformEdit, HandleComponentRemoved));
}


void TransformEdit::HandleComponentRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace ComponentRemoved;

	Component* c = (Component*)eventData[P_COMPONENT].GetPtr();
	if (c == this)
	{
		GetNode()->RemoveAllChildren();
	}

}

void TransformEdit::LoadPrimitive(Urho3D::String name)
{
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	String path = "Models/Arrow.mdl";
	if (name.Contains("Rotate"))
		path = "Models/Ring.mdl";
	Model* mdl = rc->GetResource<Model>(path);
	Material* mat = rc->GetResource<Material>("Materials/TransformEdit.xml");

	if (mdl && mat)
	{
		Node* child = GetNode()->CreateChild(name);		
		SharedPtr<Material> cloneMat = mat->Clone();
		StaticModel* sm = child->CreateComponent<StaticModel>();

		Color col = Color::WHITE;
		if (name.Contains("X"))
		{
			col = Color::RED;
			Quaternion rot = Quaternion(-90.f, Vector3::FORWARD);
			child->Rotate(rot, TS_LOCAL);
		}
		if (name.Contains("Y"))
		{
			col = Color::GREEN;
		}
		if (name.Contains("Z"))
		{
			col = Color::BLUE;
			Quaternion rot = Quaternion(90.f, Vector3::RIGHT);
			child->Rotate(rot, TS_LOCAL);
		}

		col.a_ = 0.2f;

		cloneMat->SetShaderParameter("MatDiffColor", col);
		sm->SetModel(mdl);
		sm->SetMaterial(cloneMat);
		child->SetVar("Color", col);


		//add to map
		modelMap_[sm] = name;
	}
}

IntVector2 TransformEdit::GetScaledMousePosition()
{
	IntVector2 mPos = GetSubsystem<Input>()->GetMousePosition();
	float scale = GetSubsystem<UI>()->GetScale();
	mPos.x_ = (int)(mPos.x_ * (1.0f / scale));
	mPos.y_ = (int)(mPos.y_ * (1.0f / scale));

	return mPos;
}

void TransformEdit::HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{

}

void TransformEdit::HandleMouseDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace MouseButtonDown;

	int mb = eventData[P_BUTTON].GetInt();
	
	if (mb == MOUSEB_LEFT && !editing_) {

		if (Raycast()) {

			//UI* ui = GetSubsystem<UI>();
			startScreenPos_ = GetScaledMousePosition();
			lastTransform_ = GetNode()->GetWorldTransform();
			editing_ = true;

			StaticModel* sm = (StaticModel*)raycastResult_.drawable_;
			Color col = sm->GetNode()->GetVar("Color").GetColor();
			col.a_ = 0.9f;
			sm->GetMaterial()->SetShaderParameter("MatDiffColor", col);

		}
	}
}

void TransformEdit::HandleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace MouseMove;

	if (editing_) {

		int x = eventData[P_X].GetInt();
		int y = eventData[P_Y].GetInt();

		//mpos
		//UI* ui = GetSubsystem<UI>();
		IntVector2 mPos = GetScaledMousePosition();

		IntVector2 screenDeltaVec = mPos - startScreenPos_;
		Ray screenRay = GetScreenRay(mPos);
		Vector3 sceneHint = screenRay.origin_ + raycastResult_.distance_ * screenRay.direction_;
		Vector3 sceneDeltaVec = sceneHint - raycastResult_.position_;

		//handle move command
		Vector3 axis = Vector3::ZERO;
		if (currentEditName_ == "MoveX")
		{
			axis = GetNode()->GetWorldRight();
			axis = sceneDeltaVec.ProjectOntoAxis(axis) * axis;
			Vector3 pos = lastTransform_.Translation() + axis;
			GetNode()->SetWorldPosition(pos);
		}
		if (currentEditName_ == "MoveY")
		{
			axis = GetNode()->GetWorldUp();
			axis = sceneDeltaVec.ProjectOntoAxis(axis) * axis;
			Vector3 pos = lastTransform_.Translation() + axis;
			GetNode()->SetWorldPosition(pos);
		}
		if (currentEditName_ == "MoveZ")
		{
			axis = GetNode()->GetWorldDirection();
			axis = sceneDeltaVec.ProjectOntoAxis(axis) * axis;
			Vector3 pos = lastTransform_.Translation() + axis;
			GetNode()->SetWorldPosition(pos);
		}
		if (currentEditName_ == "RotateX")
		{
			axis = GetNode()->GetWorldRight();
			Quaternion orgRot = lastTransform_.Rotation();
			Vector3 orgVec = raycastResult_.position_ - lastTransform_.Translation();
			Vector3 currVec = sceneHint - lastTransform_.Translation();
			Vector3 cross = orgVec.CrossProduct(currVec);
			float sign = Sign(cross.ProjectOntoAxis(axis));
			float angle = sign * orgVec.Angle(currVec);
			Quaternion rot = Quaternion(angle, Vector3::RIGHT);
			GetNode()->SetRotation(orgRot * rot);
		}
		if (currentEditName_ == "RotateY")
		{
			axis = GetNode()->GetWorldUp();
			Quaternion orgRot = lastTransform_.Rotation();
			Vector3 orgVec = raycastResult_.position_ - lastTransform_.Translation();
			Vector3 currVec = sceneHint - lastTransform_.Translation();
			Vector3 cross = orgVec.CrossProduct(currVec);
			float sign = Sign(cross.ProjectOntoAxis(axis));
			float angle = sign * orgVec.Angle(currVec);
			Quaternion rot = Quaternion(angle, Vector3::UP);
			GetNode()->SetRotation(orgRot * rot);
		}
		if (currentEditName_ == "RotateZ")
		{
			axis = GetNode()->GetWorldDirection();
			Quaternion orgRot = lastTransform_.Rotation();
			Vector3 orgVec = raycastResult_.position_ - lastTransform_.Translation();
			Vector3 currVec = sceneHint - lastTransform_.Translation();
			Vector3 cross = orgVec.CrossProduct(currVec);
			float sign = Sign(cross.ProjectOntoAxis(axis));
			float angle = sign * orgVec.Angle(currVec);
			Quaternion rot = Quaternion(angle, Vector3::FORWARD);
			GetNode()->SetRotation(orgRot * rot);
		}

		VariantMap data;
		data["NodePtr"] = GetNode();
		data["WorldTransform"] = GetNode()->GetWorldTransform();
		SendEvent("TransformChanged", data);

	}
}

void TransformEdit::HandleMouseUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace MouseButtonUp;

	int mb = eventData[P_BUTTON].GetInt();
	

	if (mb == MOUSEB_LEFT && editing_) {
		
		StaticModel* sm = (StaticModel*)raycastResult_.drawable_;
		Color col = sm->GetNode()->GetVar("Color").GetColor();
		col.a_ = 0.2f;
		sm->GetMaterial()->SetShaderParameter("MatDiffColor", col);
		
		editing_ = false;
		currentEditName_ = "";
		lastTransform_ = GetNode()->GetWorldTransform();
	}
}

Urho3D::Ray TransformEdit::GetScreenRay(Urho3D::IntVector2 screenPos)
{
	//try to get default viewport first
	Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();
	if (!activeViewport) {
		activeViewport = GetSubsystem<Renderer>()->GetViewport(0);
	}
	if (!activeViewport) {
		return Ray();
	}

	//also check if we are casting from inside a ui region
	UIElement* uiRegion = (UIElement*)GetGlobalVar("activeUIRegion").GetPtr();

	//get default ray
	Ray cameraRay = activeViewport->GetScreenRay(screenPos.x_, screenPos.y_);

	if (uiRegion)
	{
		IntVector2 ePos = uiRegion->GetScreenPosition();
		IntVector2 eSize = uiRegion->GetSize();
		float x = (screenPos.x_ - ePos.x_) / (float)eSize.x_;
		float y = (screenPos.y_ - ePos.y_) / (float)eSize.y_;

		cameraRay = activeViewport->GetCamera()->GetScreenRay(x, y);
	}

	return cameraRay;
}

bool TransformEdit::Raycast()
{

	//get mouse pos via ui system to account for scaling
	UI* ui = GetSubsystem<UI>();
	IntVector2 pos = GetScaledMousePosition();

	//get general screen ray, taking ui stuff in to account
	Ray cameraRay = GetScreenRay(pos);

	//do cast
	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, 100.0f, DRAWABLE_GEOMETRY);

	GetScene()->GetComponent<Octree>()->Raycast(query);

	if (results.Size() > 0)
	{
		for (int i = 0; i < results.Size(); i++)
		{
			StaticModel* sm = (StaticModel*)results[i].drawable_;
			if (sm && modelMap_.Keys().Contains(sm))
			{
				raycastResult_ = results[i];
				currentEditName_ = modelMap_[sm];
				return true;
			}
		}

	}

	return false;
}