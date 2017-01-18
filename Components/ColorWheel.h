#pragma once
//#include "AppIncludes.h"
#include "ColorDefs.h"
#include <Urho3D/UI/Sprite.h>
#include "Urho3D/UI/UIBatch.h"

class URHO3D_API ColorWheel : public Urho3D::Sprite
{
	URHO3D_OBJECT(ColorWheel, Sprite)
public:
	ColorWheel(Urho3D::Context* context);
	~ColorWheel() {};
	///segments for cuve rendering
	int segments_ = 64;
	///thickness
	float dim = 200;
	/// Return UI rendering batches.
	virtual void GetBatches(Urho3D::PODVector<Urho3D::UIBatch>& batches, Urho3D::PODVector<float>& vertexData, const Urho3D::IntRect& currentScissor);
	///compute color from position in domain [0,1] x [0,1]
	Urho3D::Color GetColorFromPosition(Urho3D::Vector2 pos);
};