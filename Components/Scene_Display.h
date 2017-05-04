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
#include <Urho3D/Graphics/BillboardSet.h>

class URHO3D_API Scene_Display : public IoComponentBase {
	URHO3D_OBJECT(Scene_Display, IoComponentBase)
public:
	Scene_Display(Urho3D::Context* context);

	virtual void PreLocalSolve();

	Urho3D::Vector<int> trackedItems;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;

	Urho3D::BillboardSet* pointCloud;
    

	///normal preview material
#ifdef EMSCRIPTEN
	Urho3D::String normalMat = "Materials/BasicWeb.xml";
	Urho3D::String normalAlphaMat = "Materials/BasicWebAlpha.xml";
#endif

#ifndef EMSCRIPTEN
	Urho3D::String normalMat = "Materials/BasicPBR.xml";
	Urho3D::String normalAlphaMat = "Materials/BasicPBRAlpha.xml";
#endif
	
	Urho3D::String normalMatWires = "Materials/BasicWireframe.xml";
	Urho3D::String pointMat = "Materials/BasicPoints.xml";
	Urho3D::String widget = "Materials/BasicTransparent.xml";
};
