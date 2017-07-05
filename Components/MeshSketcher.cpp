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

#include "MeshSketcher.h"

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

#include "TriMesh.h"


using namespace Urho3D;

MeshSketcher::MeshSketcher(Context* context) : Component(context),
editing_(false),
sketchSurface_(0)
{

}

void MeshSketcher::OnNodeSet(Urho3D::Node* node)
{
	Component::OnNodeSet(node);

	if (!node)
	{
		return;
	}

	//subscribe to events
	SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(MeshSketcher, HandleMouseDown));
	SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(MeshSketcher, HandleMouseMove));
	SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(MeshSketcher, HandleMouseUp));
	SubscribeToEvent(E_COMPONENTREMOVED, URHO3D_HANDLER(MeshSketcher, HandleComponentRemoved));
}

void MeshSketcher::Reset()
{
	curves_.Clear();
}

void MeshSketcher::HandleComponentRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace ComponentRemoved;
	URHO3D_LOGINFO("Check MB");
	Component* c = (Component*)eventData[P_COMPONENT].GetPtr();
	if (c == this)
	{

	}

}


IntVector2 MeshSketcher::GetScaledMousePosition()
{
	IntVector2 mPos = GetSubsystem<Input>()->GetMousePosition();
	float scale = GetSubsystem<UI>()->GetScale();
	mPos.x_ = (int)(mPos.x_ * (1.0f / scale));
	mPos.y_ = (int)(mPos.y_ * (1.0f / scale));

	return mPos;
}

void MeshSketcher::HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{

}

void MeshSketcher::HandleMouseDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace MouseButtonDown;

	int mb = eventData[P_BUTTON].GetInt();

	if (mb == MOUSEB_LEFT && !editing_) {

		URHO3D_LOGINFO("About to raycast..");
		if (Raycast()) {
			URHO3D_LOGINFO("Hello mouse down!");
			//UI* ui = GetSubsystem<UI>();
			startScreenPos_ = GetScaledMousePosition();
			editing_ = true;

			//start new curve
			Vector<Vector3> newCurve_;
			newCurve_.Push(raycastResult_.position_);
			curves_.Push(newCurve_);
		}
	}
}

void MeshSketcher::HandleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace MouseMove;

	if (editing_) {

		int x = eventData[P_X].GetInt();
		int y = eventData[P_Y].GetInt();

		if (Raycast())
		{
			//push
			if (!curves_.Empty())
			{
				int numCurves = curves_.Size();
				curves_[numCurves-1].Push(raycastResult_.position_);
			}
		}
	}
}

void MeshSketcher::HandleMouseUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace MouseButtonUp;

	int mb = eventData[P_BUTTON].GetInt();

	if (mb == MOUSEB_LEFT && editing_) {

		editing_ = false;

		URHO3D_LOGINFO("Hello mouse up!");

		VariantVector lastCurve;
		lastCurve.Resize(curves_.Back().Size());
		for (int i = 0; i < lastCurve.Size(); i++)
		{
			lastCurve[i] = curves_.Back()[i];
		}

		VariantMap data;
		data["MeshSketcher"] = this;
		data["LastCurve"] = lastCurve;
		SendEvent("MeshSketchChanged", data);

	}
}


bool MeshSketcher::GetScreenRay(Urho3D::IntVector2 screenPos, Urho3D::Ray& ray)
{
	//try to get default viewport first
	Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();

	if (!activeViewport)
	{
		URHO3D_LOGINFO("No active viewport found...");
		return false;
	}

	//also check if we are casting from inside a ui region
	UIElement* uiRegion = (UIElement*)GetGlobalVar("activeUIRegion").GetPtr();


	//get default ray
	ray = activeViewport->GetScreenRay(screenPos.x_, screenPos.y_);

	if (uiRegion)
	{
		IntVector2 ePos = uiRegion->GetScreenPosition();
		IntVector2 eSize = uiRegion->GetSize();
		float x = (screenPos.x_ - ePos.x_) / (float)eSize.x_;
		float y = (screenPos.y_ - ePos.y_) / (float)eSize.y_;

		ray = activeViewport->GetCamera()->GetScreenRay(x, y);
	}

	return true;
}

bool MeshSketcher::Raycast()
{

	//get mouse pos via ui system to account for scaling
	UI* ui = GetSubsystem<UI>();
	IntVector2 pos = GetScaledMousePosition();

	//get general screen ray, taking ui stuff in to account
	Ray cameraRay;
	bool res = GetScreenRay(pos, cameraRay);
	if (!res)
	{
		URHO3D_LOGINFO("Bad ray or no viewport found");
		return false;
	}


	//do cast
	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, 600.0f, DRAWABLE_GEOMETRY);

	GetScene()->GetComponent<Octree>()->Raycast(query);

	if (results.Size() > 0)
	{
		for (int i = 0; i < results.Size(); i++)
		{
			StaticModel* sm = (StaticModel*)results[i].drawable_;
			if (sm == sketchSurface_)
			{
				raycastResult_ = results[i];
				return true;
			}

		}
	}

	return false;
}
