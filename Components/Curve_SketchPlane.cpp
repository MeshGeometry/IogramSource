#include "Curve_SketchPlane.h"

#include <assert.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Graphics/Graphics.h>

#include "Polyline.h"
#include "IoGraph.h"

using namespace Urho3D;


String Curve_SketchPlane::iconTexture = "Textures/Icons/Curve_SketchPlane.png";

Curve_SketchPlane::Curve_SketchPlane(Context* context) :
	IoComponentBase(context, 0, 0)
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
		Matrix3x4::IDENTITY
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


}

void Curve_SketchPlane::HandleCustomInterface(Urho3D::UIElement* customElement)
{
	node_ = (Window*)customElement->GetParent();
	
	base_ = customElement->CreateChild<Button>("Base");
	//base_->SetStyleAuto();
	base_->SetMinSize(300, 300);

	//subscribe
	SubscribeToEvent(base_, E_DRAGBEGIN, URHO3D_HANDLER(Curve_SketchPlane, HandleDragBegin));
	SubscribeToEvent(base_, E_DRAGMOVE, URHO3D_HANDLER(Curve_SketchPlane, HandleDragMove));
	SubscribeToEvent(base_, E_DRAGEND, URHO3D_HANDLER(Curve_SketchPlane, HandleDragEnd));

	//initalize the texture
	Initialize(1024, 1024);

}


void Curve_SketchPlane::Initialize(int x, int y)
{
	//clear the pts list
	trackedPoints_.Clear();
	trackedCurves_.Clear();
	
	// set texture format
	sketchSurface_ = new Texture2D(context_);
	textureSize_ = IntVector2(x, y);
	IntVector2 baseSize = base_->GetSize();
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
			colorMap_->SetPixel(x, y, 0.1f * Color::WHITE);
		}
	}
	colorMap_->ApplyColor();

	base_->SetTexture(sketchSurface_);
}


void Curve_SketchPlane::Bresenham(int x1, int y1, int const x2, int const y2)
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

void Curve_SketchPlane::SetBrushStroke(int x, int y, int r, Urho3D::Color col)
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

void Curve_SketchPlane::HandleDragBegin(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	node_->SetMovable(false);
	trackedPoints_.Clear();

	using namespace DragBegin;
	int X = eventData[P_ELEMENTX].GetInt();
	int Y = eventData[P_ELEMENTY].GetInt();
	lastPos_ = IntVector2(X, Y);
}

void Curve_SketchPlane::HandleDragMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	
	using namespace DragMove;
	int X = eventData[P_ELEMENTX].GetInt();
	int Y = eventData[P_ELEMENTY].GetInt();
	
	IntVector2 p0((int)(textureScale_.x_ * (float)lastPos_.x_), (int)(textureScale_.y_ * (float)lastPos_.y_));
	IntVector2 p1((int)(textureScale_.x_ * (float)X), (int)(textureScale_.y_ * (float)Y));

	lastPos_ = IntVector2(X, Y);

	//track these points
	trackedPoints_.Push(Vector3(lastPos_.x_, 0.0f, lastPos_.y_));

	Bresenham(p0.x_, p0.y_, p1.x_, p1.y_);

	// update texture
	colorMap_->ApplyColor();
}

void Curve_SketchPlane::HandleDragEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	node_->SetMovable(true);

	//add last set of points to curve
	Variant lastPoly = Polyline_Make(trackedPoints_);
	trackedCurves_.Push(lastPoly);

	solvedFlag_ = 0;
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}

void Curve_SketchPlane::PreLocalSolve()
{


}

void Curve_SketchPlane::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	//get transform
	Matrix3x4 xform = inSolveInstance[3].GetMatrix3x4();
	bool reset = inSolveInstance[1].GetBool();

	if (reset)
	{
		Initialize(1024, 1024);
	}

	
    //transform
	VariantVector polysOut;
	polysOut.Resize(trackedCurves_.Size());
	for (int i = 0; i < trackedCurves_.Size(); i++)
	{
		polysOut[i] = Polyline_ApplyTransform(trackedCurves_[i], xform);;
	}

	outSolveInstance[0] = polysOut;
	outSolveInstance[1] = sketchSurface_;
}