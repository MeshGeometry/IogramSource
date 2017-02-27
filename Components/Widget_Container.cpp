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


#include "Widget_Container.h"
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>

using namespace Urho3D;

Widget_Container::Widget_Container(Urho3D::Context* context) :Widget_Base(context)
{
	URHO3D_COPY_BASE_ATTRIBUTES(Widget_Base);
}

void Widget_Container::CustomInterface()
{
	scrollView = (ScrollView*)GetChild("ScrollView", false);
	scrollContent = (Button*)GetChild("ScrollContent", true);
	if (scrollView && scrollContent)
	{
		scrollView->SetContentElement(scrollContent);
		scrollView->SetViewPosition(IntVector2(0, 0));
		scrollView->SetScrollBarsVisible(false, true);
	}

	label = (Text*)GetChild("ContainerLabel", false);

	SubscribeToEvent(this, E_RESIZED, URHO3D_HANDLER(Widget_Container, HandleResize));
	SubscribeToEvent(this, E_DRAGMOVE, URHO3D_HANDLER(Widget_Container, HandleBaseMove));
}

Widget_Container::~Widget_Container()
{

}

void Widget_Container::HandleBaseMove(StringHash eventType, VariantMap& eventData)
{
	using namespace DragMove;
    
    UI* ui = GetSubsystem<UI>();
    float curScale = ui->GetScale();
    if (curScale < 0.01)
        curScale = 1.0f;

	if (isMovable)
	{
		int DX = eventData[P_DX].GetInt();
		int DY = eventData[P_DY].GetInt();

		IntVector2 pos = GetPosition();
		SetPosition(pos + IntVector2(DX/curScale, DY/curScale));
	}

}

void Widget_Container::HandleResize(StringHash eventType, VariantMap& eventData)
{
	using namespace Resized;

	int X = eventData[P_WIDTH].GetInt();
	int Y = eventData[P_HEIGHT].GetInt();

	//resize scroll region
	if (scrollView)
	{
		scrollView->SetSize(X - 10, Y - 40);
	}

}




