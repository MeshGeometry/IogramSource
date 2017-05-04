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

#include "ReflectionProbe.h"

#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Zone.h>


//debug
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>

using namespace Urho3D;

ReflectionProbe::ReflectionProbe(Urho3D::Context* context) : 
	Component(context),
	resolution_(1024),
	initialized_(false)
{

}

bool ReflectionProbe::Initialize()
{
	//check that we have a node
	Node* node = GetNode();

	if (!node) {
		return false;
	}

	//create a new texture cube
	reflectionMap_ = new TextureCube(GetContext());
	reflectionMap_->SetSize(resolution_, Graphics::GetRGBFormat(), TEXTURE_RENDERTARGET);
	reflectionMap_->SetFilterMode(FILTER_BILINEAR);
	
	//create the camera array
	for (int i = 0; i < 6; i++) {

		//create the child nodes and cameras
		Node* camNode = node->CreateChild("__RPROBE_CAM_0__");
		cameraArray_[i] = camNode->CreateComponent<Camera>();
		cameraArray_[i]->SetFov(90.0);

		Quaternion rot = Quaternion::IDENTITY;

		//rotate the camera to cover all size sides of a cube
		RenderSurface* surface = reflectionMap_->GetRenderSurface(CubeMapFace::FACE_POSITIVE_X);
		switch (i) {
		case 0: // +X
			rot = Quaternion(90.0f, Vector3::UP);
			camNode->Rotate(rot);
			surface = reflectionMap_->GetRenderSurface(CubeMapFace::FACE_POSITIVE_X);
			break;
		case 1: // -X
			rot = Quaternion(-90.0f, Vector3::UP);
			camNode->Rotate(rot);
			surface = reflectionMap_->GetRenderSurface(CubeMapFace::FACE_NEGATIVE_X);
			break;
		case 2: // +Z
			rot = Quaternion(0.0f, Vector3::UP);
			camNode->Rotate(rot);
			surface = reflectionMap_->GetRenderSurface(CubeMapFace::FACE_POSITIVE_Z);
			break;
		case 3: // -Z
			rot = Quaternion(180.0f, Vector3::UP);
			camNode->Rotate(rot);
			surface = reflectionMap_->GetRenderSurface(CubeMapFace::FACE_NEGATIVE_Z);
			break;
		case 4: // +Y
			rot = Quaternion(-90.0f, Vector3::RIGHT);
			camNode->Rotate(rot);
			surface = reflectionMap_->GetRenderSurface(CubeMapFace::FACE_POSITIVE_Y);
			break;
		case 5: // -Y
			rot = Quaternion(90.0f, Vector3::RIGHT);
			camNode->Rotate(rot);
			surface = reflectionMap_->GetRenderSurface(CubeMapFace::FACE_NEGATIVE_Y);
			break;
		}

		//attach data to texture cube
		viewportArray_[i] = new Viewport(GetContext(), GetScene(), cameraArray_[i]);
		surface->SetViewport(0, viewportArray_[i]);
		surface->SetUpdateMode(RenderSurfaceUpdateMode::SURFACE_UPDATEALWAYS);
		
	}

	initialized_ = true;

	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(ReflectionProbe, HandleKeyDown));
	SubscribeToEvent(E_RENDERSURFACEUPDATE, URHO3D_HANDLER(ReflectionProbe, HandleRenderUpdate));

}

void ReflectionProbe::HandleRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	//UpdateReflectionMap();
}

void ReflectionProbe::UpdateReflectionMap()
{
	if (!initialized_) {
		return;
	}

	for (int i = 0; i < 6; i++) {

		CubeMapFace face = static_cast<CubeMapFace>(i);

		//debug
		//SharedPtr<Image> img = reflectionMap_->GetImage(face);
		//img->SavePNG("RTEX_" + String(i) + ".png");

		//update
		//RenderSurface* surface = reflectionMap_->GetRenderSurface(face);
		//surface->QueueUpdate();
	}

	//update zone
	Zone* zone = GetScene()->GetComponent<Zone>(true);
	if (zone) {
		zone->SetZoneTexture(reflectionMap_);
	}

}

void ReflectionProbe::HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	//using namespace KeyDown;

	//if (!initialized_) {
	//	return;
	//}

	//int key = eventData[P_KEY].GetInt();
	//int qual = eventData[P_QUALIFIERS].GetInt();
	//if (key == KEY_R ) {

	//	if (qual & QUAL_ALT) {

	//		UpdateReflectionMap();
	//	}
	//}
}