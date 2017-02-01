#pragma once

#include "IoComponentBase.h"

#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/Image.h>

class ColorMap : public Urho3D::Image
{
	//URHO3D_OBJECT(ColorMap, Image);

public:
	ColorMap(Urho3D::Context *_pContext) : Urho3D::Image(_pContext) {}
	virtual ~ColorMap() {}

	void SetSource(Urho3D::Texture2D *texture)
	{
		textureSrc_ = texture;
		SetSize(texture->GetWidth(), texture->GetHeight(), 1, texture->GetComponents());
		ApplyColor();
	}

	void ApplyColor()
	{
		textureSrc_->SetData(0, 0, 0, GetWidth(), GetHeight(), GetData());
	}

protected:
	Urho3D::WeakPtr<Urho3D::Texture2D> textureSrc_;
};

class URHO3D_API Curve_SketchPlane : public IoComponentBase {
	URHO3D_OBJECT(Curve_SketchPlane, IoComponentBase)
public:
	Curve_SketchPlane(Urho3D::Context* context);

	void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);


	static Urho3D::String iconTexture;


	Urho3D::Texture2D* sketchSurface_;
	ColorMap* colorMap_;
	Urho3D::Vector<Urho3D::String> trackedItems_;
	Urho3D::Vector<Urho3D::Variant> trackedPoints_;
	Urho3D::Vector<Urho3D::Variant> trackedCurves_;
	Urho3D::IntVector2 textureSize_;
	Urho3D::IntVector2 lastPos_;
	Urho3D::Vector2 textureScale_;
	Urho3D::Button* base_;
	Urho3D::Window* node_;

	void Initialize(int x, int y);
	void Bresenham(int x1, int y1, int const x2, int const y2);
	void SetBrushStroke(int x, int y, int r, Urho3D::Color col);
	void HandleDragBegin(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleDragMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleDragEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleCustomInterface(Urho3D::UIElement* customElement);


};