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


#include "Input_SketchPlane.h"

#include <assert.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Input/Input.h>

#include "Geomlib_ConstructTransform.h"
#include "Polyline.h"
#include "IoGraph.h"
#include "ColorDefs.h"

using namespace Urho3D;


String Input_SketchPlane::iconTexture = "Textures/Icons/Input_SketchPlane.png";

Input_SketchPlane::Input_SketchPlane(Context* context) :
	IoComponentBase(context, 0, 0),
	isActiveSketchPlane_(false),
	colorMap_(0),
	sketchSurface_(0),
	sketchPlaneNode_(0)
{
	SetName("SketchPlane");
	SetFullName("Sketch Plane");
	SetDescription("Create a sketch and position it in 3D.");

	AddInputSlot(
		"Curves",
		"CP",
		"Storage and/or optional input curves",
		VAR_VARIANTVECTOR,
		DataAccess::LIST
		);

	AddInputSlot(
		"Reset",
		"R",
		"Resets the sketch area",
		VAR_BOOL,
		DataAccess::ITEM,
		false
		);

	AddInputSlot(
		"Mode",
		"M",
		"Freeform or Line Mode",
		VAR_INT,
		DataAccess::ITEM,
		0
		);

	AddInputSlot(
		"Transform",
		"T",
		"Transform to map curve points",
		VAR_MATRIX3X4,
		DataAccess::ITEM,
		10.0f
		);

	AddOutputSlot(
		"Curves",
		"C",
		"Resulting Curves",
		VAR_VARIANTVECTOR,
		DataAccess::LIST
		);

	AddOutputSlot(
		"Texture",
		"T",
		"Sketch Texture",
		VAR_PTR,
		DataAccess::ITEM
		);

	//subscribe
	SubscribeToEvent("GraphNodeDelete", URHO3D_HANDLER(Input_SketchPlane, HandleNodeDelete));
	//SubscribeToEvent(E_CLICK, URHO3D_HANDLER(Input_SketchPlane, HandleClick));
	//SubscribeToEvent(E_DRAGBEGIN, URHO3D_HANDLER(Input_SketchPlane, HandleDragBegin));
	//SubscribeToEvent(E_DRAGMOVE, URHO3D_HANDLER(Input_SketchPlane, HandleDragMove));
	//SubscribeToEvent(E_DRAGEND, URHO3D_HANDLER(Input_SketchPlane, HandleDragEnd));

	SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(Input_SketchPlane, HandleClick));
	SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(Input_SketchPlane, HandleDragMove));
	SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(Input_SketchPlane, HandleDragEnd));

	if (!colorMap_)
	{

		URHO3D_LOGINFO("Init stuff..");
		InitializeTexture(2048, 2048);
		InitializePlane(Matrix3x4::IDENTITY);
	}


}

Input_SketchPlane::~Input_SketchPlane()
{

}

void Input_SketchPlane::HandleCustomInterface(Urho3D::UIElement* customElement)
{

	//initalize the texture
	//InitializeTexture(1024, 1024);
}


void Input_SketchPlane::InitializeTexture(int x, int y)
{
	//clear the pts list
	trackedPoints_.Clear();
	trackedCurves_.Clear();

	// set texture format
	sketchSurface_ = new Texture2D(context_);
	textureSize_ = IntVector2(x, y);
	IntVector2 baseSize = textureSize_;// base_->GetSize();
	textureScale_ = Vector2((float)textureSize_.x_ / (float)baseSize.x_, (float)textureSize_.y_ / (float)baseSize.y_);

	sketchSurface_->SetMipsToSkip(QUALITY_LOW, 0);
	sketchSurface_->SetNumLevels(1);
	sketchSurface_->SetSize(textureSize_.x_, textureSize_.y_, Graphics::GetRGBAFormat(), TEXTURE_DYNAMIC);


	colorMap_ = new ColorMap(context_);
	colorMap_->SetSource(sketchSurface_);

	for (int y = 0; y < textureSize_.y_; ++y)
	{
		for (int x = 0; x < textureSize_.x_; ++x)
		{
			colorMap_->SetPixel(x, y, 0.5f * Color::WHITE);
		}
	}
	colorMap_->ApplyColor();
}

void Input_SketchPlane::ClearTexture()
{
	if (colorMap_ && colorMap_->GetSource())
	{
		for (int y = 0; y < textureSize_.y_; ++y)
		{
			for (int x = 0; x < textureSize_.x_; ++x)
			{
				colorMap_->SetPixel(x, y, 0.5f * Color::WHITE);
			}
		}
		colorMap_->ApplyColor();
	}
}

void Input_SketchPlane::InitializePlane(Urho3D::Matrix3x4 transform)
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (!scene)
	{
		return;
	}

	ResourceCache* rc = GetSubsystem<ResourceCache>();
	Model* mdl = rc->GetResource<Model>("Models/Plane.mdl");
	Material* mat = rc->GetResource<Material>("Materials/BasicDiffuseAlpha.xml");


	if (mdl && mat)
	{
		sketchPlaneNode_ = scene->CreateChild("PlaneNode");
		sketchPlaneNode_->SetWorldTransform(transform.Translation(), transform.Rotation(), transform.Scale());
		sketchPlaneNode_->SetScale(10.0f * Vector3::ONE);

		SharedPtr<Material> cloneMat = mat->Clone();
		cloneMat->SetShaderParameter("MatDiffColor", 0.5f * Color::WHITE);
		StaticModel* sm = sketchPlaneNode_->CreateComponent<StaticModel>();
		sm->SetModel(mdl);


		if (sketchSurface_)
		{
			cloneMat->SetTexture(TextureUnit::TU_DIFFUSE, sketchSurface_);
		}

		sm->SetMaterial(cloneMat);
	}
}

void Input_SketchPlane::Bresenham(int x1, int y1, int const x2, int const y2)
{
	int delta_x(x2 - x1);
	// if x1 == x2, then it does not matter what we set here
	signed char const ix((delta_x > 0) - (delta_x < 0));
	delta_x = std::abs(delta_x) << 1;

	int delta_y(y2 - y1);
	// if y1 == y2, then it does not matter what we set here
	signed char const iy((delta_y > 0) - (delta_y < 0));
	delta_y = std::abs(delta_y) << 1;

	Color col(0.5, 0.5, 0.5, 1.0);

	//plot(x1, y1);
	//colorMap_->SetPixel(x1, y1, col);
	SetBrushStroke(x1, y1, 4, col);
	if (delta_x >= delta_y)
	{
		// error may go below zero
		int error(delta_y - (delta_x >> 1));

		while (x1 != x2)
		{
			if ((error >= 0) && (error || (ix > 0)))
			{
				error -= delta_x;
				y1 += iy;
			}
			// else do nothing

			error += delta_y;
			x1 += ix;

			//plot(x1, y1);
			//colorMap_->SetPixel(x1, y1, col);
			SetBrushStroke(x1, y1, 4, col);
		}
	}
	else
	{
		// error may go below zero
		int error(delta_x - (delta_y >> 1));

		while (y1 != y2)
		{
			if ((error >= 0) && (error || (iy > 0)))
			{
				error -= delta_y;
				x1 += ix;
			}
			// else do nothing

			error += delta_x;
			y1 += iy;

			//plot(x1, y1);
			//colorMap_->SetPixel(x1, y1, col);
			SetBrushStroke(x1, y1, 4, col);
		}
	}
}

void Input_SketchPlane::SetBrushStroke(int x, int y, int r, Urho3D::Color col)
{
	if (!colorMap_)
		return;

	float rsq = Pow(r, 2);
	for (int i = -r; i <= r; i++)
	{
		for (int j = -r; j <= r; j++)
		{
			int nx = x + i;
			int ny = y + j;

			float dsq = Pow(nx - x, 2) + Pow(ny - y, 2);
			if (dsq < rsq)
			{
				float alpha = col.a_;
				col.a_ = (1.f - SmoothStep(0.8f * rsq, rsq, dsq)) * col.a_;

				//Color currPix = colorMap_->GetPixel(nx, ny);
				//currPix += col;
				colorMap_->SetPixel(nx, ny, col);
			}
		}
	}
}

bool Input_SketchPlane::DoRaycast(Urho3D::IntVector2 pos, Urho3D::RayQueryResult& result, bool forceLocal)
{
	//use delta and object transform to move object
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();
	if (!activeViewport || !scene)
	{
		URHO3D_LOGINFO("Could not get active viewport");
		return false;
	}

	int X = pos.x_;
	int Y = pos.y_;

	Graphics* graphics = GetSubsystem<Graphics>();
	Camera* camera = activeViewport->GetCamera();

	//create default cast position
	Ray ray = camera->GetScreenRay(X / graphics->GetWidth(), Y / graphics->GetHeight());

	//remap mouse pos by ui rect
	UIElement* element = (UIElement*)GetGlobalVar("activeUIRegion").GetPtr();
	if (element)
	{
		IntVector2 ePos = element->GetScreenPosition();
		IntVector2 eSize = element->GetSize();
		float sx = (X - ePos.x_) / (float)eSize.x_;
		float sy = (Y - ePos.y_) / (float)eSize.y_;

		ray = camera->GetScreenRay(sx, sy);
	}

	//do raycast
	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, ray, RAY_TRIANGLE_UV, 100.0f, RayQueryLevel::RAY_TRIANGLE_UV);

	if (!sketchPlaneNode_)
	{
		URHO3D_LOGINFO("Raycast fail");
		return false;
	}


	if (forceLocal)
	{
		StaticModel* sm = sketchPlaneNode_->GetComponent<StaticModel>();
		sm->ProcessRayQuery(query, results);
	}
	else
	{
		scene->GetComponent<Octree>()->RaycastSingle(query);
	}

	//loop through results and match up with tracked planes
	for (int i = 0; i < results.Size(); i++)
	{
		if (results[i].node_ == sketchPlaneNode_)
		{
			result = results[i];

			URHO3D_LOGINFO("Raycast success");
			return true;

		}
	}

	URHO3D_LOGINFO("Raycast fail");
	return false;
}

void Input_SketchPlane::HandleDragBegin(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{

	using namespace DragBegin;
	int X = eventData[P_X].GetInt();
	int Y = eventData[P_Y].GetInt();

	if (eventData[P_BUTTONS].GetInt() & MOUSEB_LEFT)
	{
		RayQueryResult result;
		if (DoRaycast(IntVector2(X, Y), result))
		{
			lastHitResult_ = result;
		}
	}


}

void Input_SketchPlane::HandleDragMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{

	using namespace MouseMove;
	int X = eventData[P_X].GetInt();
	int Y = eventData[P_Y].GetInt();

	IntVector2 mPos = GetSubsystem<Input>()->GetMousePosition();
	float scale = GetSubsystem<UI>()->GetScale();
	mPos.x_ = (int)(mPos.x_ * (1.0f / scale));
	mPos.y_ = (int)(mPos.y_ * (1.0f / scale));

	if (eventData[P_BUTTONS].GetInt() & MOUSEB_LEFT && isActiveSketchPlane_)
	{
		RayQueryResult result;
		if (DoRaycast(mPos, result, true))
		{

			IntVector2 p0((int)(textureSize_.x_ * (float)lastHitResult_.textureUV_.x_), (int)(textureSize_.y_ * (float)lastHitResult_.textureUV_.y_));
			IntVector2 p1((int)(textureSize_.x_ * (float)result.textureUV_.x_), (int)(textureSize_.y_ * (float)result.textureUV_.y_));
			Bresenham(p0.x_, p0.y_, p1.x_, p1.y_);
			lastHitResult_ = result;

			//push to curve stack
			trackedPoints_.Push(result.position_);

			// update texture
			colorMap_->ApplyColor();
		}
	}

}

void Input_SketchPlane::HandleDragEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace MouseButtonUp;
	int isLeftMouse = eventData[P_BUTTON].GetInt() & MOUSEB_LEFT;
	if (isLeftMouse && isActiveSketchPlane_)
	{
		if (trackedPoints_.Size() > 1)
		{
			//add last set of points to curve
			Variant lastPoly = Polyline_Make(trackedPoints_);
			
			//transform to local space
			Variant localPoly = Polyline_ApplyTransform(lastPoly, sketchPlaneNode_->GetWorldTransform().Inverse());

			trackedCurves_.Push(localPoly);

			//clear
			trackedPoints_.Clear();

			solvedFlag_ = 0;
			GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
		}
	}
}

void Input_SketchPlane::UpdateSketchPlaneTransform(Urho3D::Matrix3x4 xform)
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		if (sketchPlaneNode_)
		{
			sketchPlaneNode_->SetWorldTransform(xform.Translation(), xform.Rotation(), xform.Scale());
		}
	}
}

void Input_SketchPlane::HandleNodeDelete(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	IoComponentBase* comp = (IoComponentBase*)eventData["GraphNode"].GetPtr();
	if (comp == this)
	{
		URHO3D_LOGINFO("deleting....");
		if (sketchPlaneNode_)
		{
			sketchPlaneNode_->Remove();
		}

		UnsubscribeFromAllEvents();
	}
}

void Input_SketchPlane::HandleClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace MouseButtonDown;

	int isLeftClick = eventData[P_BUTTON].GetInt() & MOUSEB_LEFT;
	IntVector2 mPos = GetSubsystem<Input>()->GetMousePosition();
	float scale = GetSubsystem<UI>()->GetScale();
	mPos.x_ = (int)(mPos.x_ * (1.0f / scale));
	mPos.y_ = (int)(mPos.y_ * (1.0f / scale));
	if (isLeftClick)
	{
		RayQueryResult result;
		if (DoRaycast(mPos, result))
		{
			
			URHO3D_LOGINFO("Active sketch plane found...");

			isActiveSketchPlane_ = true;
			lastHitResult_ = result;

			if (sketchPlaneNode_)
			{
				StaticModel* sm = sketchPlaneNode_->GetComponent<StaticModel>();
				Material* mat = sm->GetMaterial();
				mat->SetShaderParameter("MatDiffColor", 0.5f * LIGHT_GREEN);
			}
		}
		else
		{
			isActiveSketchPlane_ = false;

			if (sketchPlaneNode_)
			{
				StaticModel* sm = sketchPlaneNode_->GetComponent<StaticModel>();
				Material* mat = sm->GetMaterial();
				mat->SetShaderParameter("MatDiffColor", 0.5f * Color::WHITE);
			}
		}
	}
}


void Input_SketchPlane::PreLocalSolve()
{


}

void Input_SketchPlane::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	)
{
	URHO3D_LOGINFO("Solving...");
	
	//get transform
	Matrix3x4 xform = Geomlib::ConstructTransform(inSolveInstance[3]);
	bool reset = inSolveInstance[1].GetBool();

	if (reset)
	{
		URHO3D_LOGINFO("clearing..");
		ClearTexture();
		trackedCurves_.Clear();
	}

	UpdateSketchPlaneTransform(xform);

	//transform the curves to world space
	VariantVector newCurves;
	newCurves.Resize(trackedCurves_.Size());
	for (int j = 0; j < trackedCurves_.Size(); j++)
	{
		Variant nc = Polyline_ApplyTransform(trackedCurves_[j], xform);
		newCurves[j] = nc;
	}

	if (!sketchSurface_ || trackedCurves_.Empty())
	{
		SetAllOutputsNull(outSolveInstance);
		URHO3D_LOGINFO("returning null....");
		return;
	}

	outSolveInstance[0] = newCurves;
	outSolveInstance[1] = sketchSurface_;
}