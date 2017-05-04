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