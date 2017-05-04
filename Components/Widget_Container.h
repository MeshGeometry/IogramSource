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
#include "Widget_Base.h"

#include <Urho3D/UI/ScrollView.h>
#include <Urho3D/UI/Text.h>

class URHO3D_API Widget_Container : public Widget_Base {

	URHO3D_OBJECT(Widget_Container, Widget_Base)

public:
	Widget_Container(Urho3D::Context* context);
	~Widget_Container();
	void CustomInterface();
	Urho3D::Button* GetContentElement() { return scrollContent; };
	Urho3D::ScrollView* GetScrollElement() { return scrollView; };
	Urho3D::Text* GetTextElement() { return label; };

	bool isMovable = false;
	bool useAutoLayout = true;
private:
	Urho3D::Button* scrollContent;
	Urho3D::ScrollView* scrollView;
	Urho3D::Text* label;

	void HandleBaseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleResize(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};