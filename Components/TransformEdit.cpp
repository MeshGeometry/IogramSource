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
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

TransformEdit::TransformEdit(Context* context) : Component(context),
	lineScale_(1.0f),
	transformHandles_(0),
	editing_(false)
{

}

void TransformEdit::OnNodeSet(Urho3D::Node* node)
{
	Component::OnNodeSet(node);

	//perform start up ops
	Vector<Vector3> moveDirs;
	moveDirs.Push(Vector3::RIGHT);
	moveDirs.Push(Vector3::UP);
	moveDirs.Push(Vector3::FORWARD);

	//create viz
	SetMoveAxes(moveDirs);

	//subscribe to events
	SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(TransformEdit, HandleMouseDown));
	SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(TransformEdit, HandleMouseMove));
	SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(TransformEdit, HandleMouseUp));

}

void TransformEdit::SetMoveAxes(Urho3D::Vector<Urho3D::Vector3> axes)
{
	//clear exisiting
	moveAxes_.Clear();

	//set
	moveAxes_ = axes;

	//rebuild gizmo
	CreateTransformGizmo();
}

void TransformEdit::CreateTransformGizmo()
{
	if (!GetNode()) {
		return;
	}
	
	if (transformHandles_ && transformLines_) {
		transformHandles_->Remove();
		transformLines_->Remove();
	}

	//get a material
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	Material* mat = rc->GetResource<Material>("Materials/DefaultGrey.xml");
	
	//init the billboard set
	transformHandles_ = GetNode()->CreateComponent<BillboardSet>();
	transformHandles_->SetSorted(true);
	transformHandles_->SetFaceCameraMode(FaceCameraMode::FC_DIRECTION);
	transformHandles_->SetMaterial(mat);

	transformLines_ = GetNode()->CreateComponent<BillboardSet>();
	transformLines_->SetSorted(true);
	transformLines_->SetFaceCameraMode(FaceCameraMode::FC_DIRECTION);
	transformLines_->SetMaterial(mat);
	
	//init the billboards
	transformHandles_->SetNumBillboards(moveAxes_.Size());
	transformLines_->SetNumBillboards(moveAxes_.Size());

	//create the move gizmos
	for (int i = 0; i < moveAxes_.Size(); i++) {
		CreateMoveGizmo(moveAxes_[i], i);
	}

}

Billboard* TransformEdit::CreateMoveGizmo(Vector3 direction, int id)
{
	if (!transformHandles_) {
		return 0;
	}

	assert(id < transformHandles_->GetNumBillboards());
	assert(id < transformLines_->GetNumBillboards());

	direction.Normalize();

	//actually create the move gizmo now...
	Billboard* bb = transformLines_->GetBillboard(id);
	bb->position_ = lineScale_ *  0.5f * direction;
	bb->size_ = Vector2(0.01f * lineScale_, lineScale_ *  0.5f);
	bb->direction_ = direction;
	bb->enabled_ = true;
	bb->color_ = Color(direction.x_, direction.y_, direction.z_, 1.0f);

	bb = transformHandles_->GetBillboard(id);
	bb->position_ = lineScale_ * direction + 0.1f * lineScale_* direction;
	bb->size_ = Vector2(0.1f * lineScale_, 0.1f * lineScale_);
	bb->direction_ = direction;
	bb->enabled_ = true;
	bb->color_ = Color(direction.x_, direction.y_, direction.z_, 1.0f);

	return bb;
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

			editing_ = true;
		}
	}
}

void TransformEdit::HandleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace MouseMove;

	if (editing_) {

		int x = eventData[P_X].GetInt();
		int y = eventData[P_Y].GetInt();

		//scale 
		float scale = GetSubsystem<UI>()->GetScale();
		x = x / scale;
		y = y / scale;

		//move node based on length and billboard directions
		Billboard* bb = transformHandles_->GetBillboard(raycastResult_.subObject_);
		if (bb) {

			//try to get default viewport first
			Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();
			if (!activeViewport) {
				activeViewport = GetSubsystem<Renderer>()->GetViewport(0);
			}
			if (!activeViewport) {
				return;
			}

			Camera* currentCamera = activeViewport->GetCamera();
			Vector3 camToNode = raycastResult_.position_ - currentCamera->GetNode()->GetWorldPosition();
			float angle = camToNode.Angle(currentCamera->GetNode()->GetWorldDirection());
			float length = camToNode.Length() / Cos(angle);

			//create default cast position
			Vector3 newPos = activeViewport->ScreenToWorldPoint(x, y, length);

			//also check if we are casting from inside a ui region
			UIElement* uiRegion = (UIElement*)GetGlobalVar("activeUIRegion").GetPtr();

			if (uiRegion)
			{
				IntVector2 ePos = uiRegion->GetScreenPosition();
				IntVector2 eSize = uiRegion->GetSize();
				float sx = (x - ePos.x_) / (float)eSize.x_;
				float sy = (y - ePos.y_) / (float)eSize.y_;

				newPos = currentCamera->ScreenToWorldPoint(Vector3(sx, sy, length));
			}

			//get constrained vector
			Vector3 moveVec = newPos - bb->position_;
			Vector3 projVec = moveVec.ProjectOntoAxis(bb->direction_) * bb->direction_;

			//finally move;
			GetNode()->Translate(projVec);
		}
	}
}

void TransformEdit::HandleMouseUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace MouseButtonUp;

	int mb = eventData[P_BUTTON].GetInt();
	

	if (mb == MOUSEB_LEFT) {
		editing_ = false;
	}
}

bool TransformEdit::Raycast()
{
	
	//try to get default viewport first
	Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();
	if (!activeViewport) {
		activeViewport = GetSubsystem<Renderer>()->GetViewport(0);
	}
	if (!activeViewport) {
		return false;
	}

	//get mouse pos via ui system to account for scaling
	UI* ui = GetSubsystem<UI>();
	IntVector2 pos = ui->GetCursorPosition();

	//also check if we are casting from inside a ui region
	UIElement* uiRegion = (UIElement*)GetGlobalVar("activeUIRegion").GetPtr();

	//get default ray
	Ray cameraRay = activeViewport->GetScreenRay(pos.x_, pos.y_);

	if (uiRegion)
	{
		IntVector2 ePos = uiRegion->GetScreenPosition();
		IntVector2 eSize = uiRegion->GetSize();
		float x = (pos.x_ - ePos.x_) / (float)eSize.x_;
		float y = (pos.y_ - ePos.y_) / (float)eSize.y_;

		cameraRay = activeViewport->GetCamera()->GetScreenRay(x, y);
	}

	//do cast
	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, 1000.0f, DRAWABLE_GEOMETRY);

	//Scene* scene = GetNode()->GetScene();
	//scene->GetComponent<Octree>()->RaycastSingle(query);
	transformHandles_->ProcessRayQuery(query, results);

	if (results.Size() > 0)
	{

		if (results[0].drawable_ = transformHandles_) {
			raycastResult_ = results.At(0);

			URHO3D_LOGINFO("Got one!");

			return true;
		}
	}

	return false;
}