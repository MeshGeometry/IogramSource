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

#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/MultiLineEdit.h>
#include <Urho3D/UI/DropDownList.h>

#include "IoComponentBase.h"

class URHO3D_API Input_Panel : public IoComponentBase {

	URHO3D_OBJECT(Input_Panel, IoComponentBase)

public:
	Input_Panel(Urho3D::Context* context);

public:

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;

	Urho3D::SharedPtr<Urho3D::MultiLineEdit> textArea_;
	Urho3D::SharedPtr<Urho3D::LineEdit> targetType_;

	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);
	bool editable_;

	void SetDataTree();
	void SetDataTreeContent();
	void CastPanelContents();

	void HandleLineEditCommit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleGraphSolve(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleSetText(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleDefocus(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleFocus(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleTargetTypeChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

};