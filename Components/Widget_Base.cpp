#include "Widget_Base.h"

using namespace Urho3D;

Widget_Base::Widget_Base(Urho3D::Context* context) : Button(context)
{
	URHO3D_COPY_BASE_ATTRIBUTES(Button);
}

void Widget_Base::HandleResize(StringHash eventType, VariantMap& eventData)
{

}
