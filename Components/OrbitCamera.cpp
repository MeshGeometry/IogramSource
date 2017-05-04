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


#include "OrbitCamera.h"
#include <Urho3D/Input/Input.h>
#include <Urho3D/UI/UI.h>
#include "Urho3D/Physics/PhysicsUtils.h"
#include "Urho3D/Physics/PhysicsWorld.h"
#include "Urho3D/Graphics/Material.h"
#include "Urho3D/Graphics/Model.h"
#include "Urho3D/Graphics/StaticModel.h"
#include "Urho3D/Graphics/Camera.h"
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

OrbitCamera::OrbitCamera(Context* context) :
	LogicComponent(context)
{

	// Only the scene update event is needed: unsubscribe from the rest for optimization
	SetUpdateEventMask(USE_UPDATE);
}

void OrbitCamera::Start()
{
	GetNode()->SetPosition(Vector3::ZERO);

	//cam setup
	cam_node_ = GetNode()->CreateChild("CamNode");
	cam_node_->SetPosition(Vector3(0.0f, 0.0f, -20.0f));
	cam_node_->LookAt(GetNode()->GetWorldPosition());
	camera_ = cam_node_->CreateComponent<Camera>();
	camera_->SetFarClip(1000.0f);
	camera_->SetFov(45.0f);

	//debug
	//ResourceCache* cache = GetSubsystem<ResourceCache>();
	//Model* m = cache->GetResource<Model>("Models/Box.mdl");
	//StaticModel* sm = GetNode()->CreateComponent<StaticModel>();
	//sm->SetModel(m);

	int viewMask = camera_->GetViewMask();

	// Create a directional light
	//Light* light;
	//light_node_ = GetNode()->CreateChild("DirectionalLight");
	//light = cam_node_->CreateComponent<Light>();
	////light_node_->SetPosition(Vector3(0.0f, 10.0f, 20.f));
	////light_node_->LookAt(GetNode()->GetWorldPosition());
	//light->SetColor(Color(0.8f, 0.8f, 0.9f));
	//light->SetBrightness(2.f);
	//light->SetLightType(LightType::LIGHT_SPOT);
	//light->SetRange(100.f);
	//light->SetCastShadows(true);
}

void OrbitCamera::Update(float timeStep)
{
	// Do not move if the UI has a focused element (the console)
	//if (GetSubsystem<UI>()->GetFocusElement())
	//	return;

	// Do not move if the UI has a focused element (the console)
	//if (GetSubsystem<UI>()->GetFocusElement())
	//	return;

	Input* input = GetSubsystem<Input>();

	// Movement speed as world units per second
	const float MOVE_SPEED = 40.0f;
	// Mouse sensitivity as degrees per pixel
	const float MOUSE_SENSITIVITY = 0.1f;

	// Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
	IntVector2 mouseMove = input->GetMouseMove();
	float d = cam_node_->GetPosition().Length();
	float t = Min(100.0f, d) / 100.0f;
	//double d_mult = Lerp(0.01f, 1.0f, t);
	float moveX = MOUSE_SENSITIVITY * mouseMove.x_;
	float moveY = MOUSE_SENSITIVITY * mouseMove.y_;
	//Vector3 zoomVec = GetNode()->GetWorldPosition() - cam_node_->GetWorldPosition();

	if (camera_ == NULL)
		return;

	if (input->GetMouseButtonDown(MOUSEB_RIGHT))
	{
		if (input->GetKeyDown(KEY_LSHIFT))
		{
			//pan
			Vector3 move_vec = 0.01 * d * (moveY * cam_node_->GetWorldUp() - moveX * cam_node_->GetWorldRight());
			GetNode()->Translate(move_vec, TS_WORLD);

		}

		else if (input->GetKeyDown(KEY_LCTRL))
		{
			//zoom
			float factor = 0.1f;
			if (camera_->IsOrthographic())
			{
				float oSize = camera_->GetOrthoSize();
				oSize += factor* Sign(moveY);
				camera_->SetOrthoSize(oSize);
			}
			else
			{
				if (d < 1.0f)
				{
					factor = factor * Pow(d, 1.0f);
				}

				if (Sign(moveY) < 0)
				{
					factor = Max(factor, 0.1f);
				}

				cam_node_->Translate(factor* Sign(moveY) * d * Vector3::FORWARD, TS_LOCAL);
			}
		}

		else
		{
			//rotate
			GetNode()->RotateAround(GetNode()->GetPosition(), Quaternion(moveX, GetNode()->GetUp()), TS_WORLD);
			cam_node_->RotateAround(GetNode()->GetPosition(), Quaternion(moveY, GetNode()->GetRight()), TS_WORLD);
		}
	}


	if (input->GetKeyDown(KEY_LCTRL) && input->GetKeyPress('w'))
	{
		if (camera_->GetFillMode() == FILL_SOLID)
			camera_->SetFillMode(FILL_WIREFRAME);
		else
			camera_->SetFillMode(FILL_SOLID);

		Node* sky = GetScene()->GetChild("Sky", false);
		if (sky)
		{
			(sky->IsEnabled()) ? sky->SetEnabled(false) : sky->SetEnabled(true);
		}
	}

	if (input->GetKeyDown(KEY_LCTRL) && input->GetKeyPress('p'))
	{
		if (camera_->IsOrthographic())
		{
			camera_->SetOrthographic(false);
		}
		else
		{
			camera_->SetOrthographic(true);
		}
	}

	if (input->GetKeyDown(KEY_LCTRL) && input->GetKeyPress('l'))
	{
		if (light_node_)
			light_node_->SetEnabled(!light_node_->IsEnabled());
	}

	if (input->GetKeyDown(KEY_TAB) && input->GetKeyPress('c'))
		GetNode()->SetPosition(Vector3::ZERO);

	if (input->GetKeyDown(KEY_LCTRL))
	{
		//view keyboard shortcuts
		//if (input->GetKeyPress('5'))
		//	SetView(CameraViews::TOP);
		//if (input->GetKeyPress('6'))
		//	SetView(CameraViews::RIGHT);
		//if (input->GetKeyPress('4'))
		//	SetView(CameraViews::LEFT);
		//if (input->GetKeyPress('2'))
		//	SetView(CameraViews::FRONT);
		//if (input->GetKeyPress('8'))
		//	SetView(CameraViews::BACK);
		//if (input->GetKeyPress('1'))
		//	SetView(CameraViews::BOTTOM);

		if (input->GetKeyPress('e'))
			ZoomExtents();
	}

	if (input->GetMouseButtonPress(MOUSEB_RIGHT))
	{

	}

}

void OrbitCamera::SetView(CameraViews view)
{
	//set cam position to origin
	//GetNode()->SetPosition(Vector3::ZERO);
	Quaternion rot(Quaternion::IDENTITY);
	switch (view)
	{
	case CameraViews::TOP:
	{
		rot = Quaternion(90, Vector3::RIGHT);
		GetNode()->SetWorldRotation(rot);
		break;
	}
	case CameraViews::BOTTOM:
	{
		rot = Quaternion(-90, Vector3::RIGHT);
		GetNode()->SetWorldRotation(rot);
		break;
	}
	case CameraViews::RIGHT:
	{
		rot = Quaternion(90, Vector3::UP);
		GetNode()->SetWorldRotation(rot);
		break;
	}
	case CameraViews::LEFT:
	{
		rot = Quaternion(-90, Vector3::UP);
		GetNode()->SetWorldRotation(rot);
		break;
	}
	case CameraViews::FRONT:
	{
		rot = Quaternion(0, Vector3::UP);
		GetNode()->SetWorldRotation(rot);
		break;
	}
	case CameraViews::BACK:
	{
		rot = Quaternion(180, Vector3::UP);
		GetNode()->SetWorldRotation(rot);
		break;
	}
	default:
		URHO3D_LOGINFO("no camera view of that type implemented");
		break;
	}
}

void OrbitCamera::ZoomExtents()
{
	Scene* scene = GetScene();
	PODVector<Node*> all_nodes;
	scene->GetChildren(all_nodes, true);

	Vector3 min = -Vector3::ONE;
	Vector3 max = Vector3::ONE;

	for (int i = 0; i < all_nodes.Size(); i++)
	{
		Node* currNode = all_nodes.At(i);
		if (!currNode->HasComponent<StaticModel>() || !currNode->IsEnabled() || currNode->GetName() == "grid")
			continue;

		StaticModel* curr_model = currNode->GetComponent<StaticModel>();
		BoundingBox b = curr_model->GetBoundingBox();

		min.x_ = Min(min.x_, b.min_.x_);
		min.y_ = Min(min.y_, b.min_.y_);
		min.z_ = Min(min.z_, b.min_.z_);

		max.x_ = Max(max.x_, b.max_.x_);
		max.y_ = Max(max.y_, b.max_.y_);
		max.z_ = Max(max.z_, b.max_.z_);
	}

	ZoomExtents(min, max);
}


void OrbitCamera::ZoomExtents(Vector3 min, Vector3 max)
{
	//center on midpoint
	Vector3 midPoint = 0.5*(min + max);
	GetNode()->SetPosition(midPoint);

	//Select the pt to use for zooming
	Vector2 minScreenPos = camera_->WorldToScreenPoint(min);
	Vector2 maxScreenPos = camera_->WorldToScreenPoint(max);
	Vector2 center(0.5, 0.5);
	Vector2 minReCenter = minScreenPos - center;
	Vector3 maxReCenter = maxScreenPos - center;

	Vector3 zoomPt = max;
	if (minReCenter.Length() > maxReCenter.Length())
		zoomPt = min;

	//proceed with the trig
	Vector3 camPos = cam_node_->GetWorldPosition();
	Vector3 camDir = cam_node_->GetWorldDirection();

	Vector3 camToPt = zoomPt - camPos;
	Vector3 camToCenter = camToPt.DotProduct(camDir) * camDir;
	Vector3 perpVec = zoomPt - (camPos + camToCenter);
	double totalZoomLength = perpVec.Length() / Tan(0.5 * camera_->GetFov());

	cam_node_->Translate((camToCenter.Length() - totalZoomLength)*Vector3::FORWARD, TS_LOCAL);
}
