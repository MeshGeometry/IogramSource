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

#include "IoComponentBase.h"

#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Scene/Scene.h>

#include "Curve_SketchPlane.h"

class URHO3D_API Input_SketchPlane : public IoComponentBase {
	URHO3D_OBJECT(Input_SketchPlane, IoComponentBase)
public:
	Input_SketchPlane(Urho3D::Context* context);
	~Input_SketchPlane();

	void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);


	static Urho3D::String iconTexture;



	Urho3D::Texture2D* sketchSurface_;
	ColorMap* colorMap_;
	Urho3D::Vector<Urho3D::String> trackedItems_;
	Urho3D::Vector<int> trackedNodes_;
	Urho3D::Vector<Urho3D::Variant> trackedPoints_;
	Urho3D::Vector<Urho3D::Variant> trackedCurves_;
	Urho3D::IntVector2 textureSize_;
	Urho3D::IntVector2 lastPos_;
	Urho3D::Vector2 textureScale_;
	Urho3D::RayQueryResult lastHitResult_;
	bool isActiveSketchPlane_;
	Urho3D::Node* sketchPlaneNode_;

	void ClearTexture();
	void UpdateSketchPlaneTransform(Urho3D::Matrix3x4 xform);
	bool DoRaycast(Urho3D::IntVector2 pos, Urho3D::RayQueryResult& result, bool forceLocal=false);
	void InitializeTexture(int x, int y);
	void InitializePlane(Urho3D::Matrix3x4 transform);
	void Bresenham(int x1, int y1, int const x2, int const y2);
	void SetBrushStroke(int x, int y, int r, Urho3D::Color col);
	void HandleDragBegin(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleDragMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleDragEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleCustomInterface(Urho3D::UIElement* customElement);
	void HandleNodeDelete(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

};