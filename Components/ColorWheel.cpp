#include "ColorWheel.h"

#include <Urho3D/Core/Context.h>

using namespace Urho3D;

ColorWheel::ColorWheel(Context* context) : Sprite(context)
{
	context->RegisterFactory<ColorWheel>();
}

void ColorWheel::GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor)
{
	//some preamble
	bool allOpaque = true;
	if (GetDerivedOpacity() < 1.0f || color_[C_TOPLEFT].a_ < 1.0f || color_[C_TOPRIGHT].a_ < 1.0f ||
		color_[C_BOTTOMLEFT].a_ < 1.0f || color_[C_BOTTOMRIGHT].a_ < 1.0f)
		allOpaque = false;

	//create a new batch
	UIBatch batch(this, blendMode_ == BLEND_REPLACE && !allOpaque ? BLEND_ALPHA : blendMode_,
		currentScissor,
		texture_,
		&vertexData);


	//create segments of wheel
	float angle = (360.0f) / segments_;

	//some settings
	dim = Min(GetSize().x_, GetSize().y_) / 2.0f;
	float offset = 0.0f;
	int innerOffset = dim/10;

	for (int i = 0; i < segments_; i++)
	{
		//TODO: figure out color gradient
		float val = ((float)i / segments_);
		Color currCol(val, 1.0f - val, 1.0f - val, 1.0f);

		//figure out quad verts

		Vector2 vecA(Cos(i * angle + offset), Sin(i * angle + offset));
		Vector2 vecB(Cos((i + 1) * angle - offset), Sin((i + 1) * angle - offset));

		IntVector2 a(innerOffset * vecA.x_, innerOffset * vecA.y_);
		IntVector2 b(dim * vecA.x_, dim * vecA.y_);
		IntVector2 c(dim * vecB.x_, dim * vecB.y_);
		IntVector2 d(innerOffset * vecB.x_, innerOffset * vecB.y_);

		unsigned int col = batch.color_;
		batch.AddQuad(GetTransform(), a, b, c, d, 
			IntVector2(0, 0), IntVector2(1, 0), IntVector2(1, 1), IntVector2(0, 1),
			currCol, currCol, currCol, currCol);

	}

	UIBatch::AddOrMerge(batch, batches);

	// Reset hovering for next frame
	hovering_ = false;
}

Color ColorWheel::GetColorFromPosition(Vector2 pos)
{
	return Color::BLACK;
}