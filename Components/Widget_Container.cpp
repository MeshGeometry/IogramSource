#include "Widget_Container.h"
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>

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

	if (isMovable)
	{
		int DX = eventData[P_DX].GetInt();
		int DY = eventData[P_DY].GetInt();

		IntVector2 pos = GetPosition();
		SetPosition(pos + IntVector2(DX, DY));
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




