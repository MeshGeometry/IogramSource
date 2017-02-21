#include "Input_MouseDownListener.h"

#include "IoGraph.h"

#include <Urho3D/UI/UIEvents.h>
#include "Urho3D/Graphics/Viewport.h"
#include "Urho3D/Physics/PhysicsWorld.h"
#include <Urho3D/Input/Input.h>
#include <Urho3D/UI/UI.h>

using namespace Urho3D;

String Input_MouseDownListener::iconTexture = "Textures/Icons/Input_MouseDownListener.png";

Input_MouseDownListener::Input_MouseDownListener(Urho3D::Context* context) : IoComponentBase(context, 2, 2)
{
	SetName("MouseDownListener");
	SetFullName("Mouse Down Listener");
	SetDescription("Listens for mouse down in scene");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("INPUT");

	inputSlots_[0]->SetName("On");
	inputSlots_[0]->SetVariableName("On");
	inputSlots_[0]->SetDescription("Listen for mouse clicks");
	inputSlots_[0]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(true);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Mouse button");
	inputSlots_[1]->SetVariableName("MB");
	inputSlots_[1]->SetDescription("Mouse Button");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(1);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Mouse Position");
	outputSlots_[0]->SetVariableName("MP");
	outputSlots_[0]->SetDescription("Mouse Position");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Mouse Delta");
	outputSlots_[1]->SetVariableName("MD");
	outputSlots_[1]->SetDescription("Mouse Delta");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}


void Input_MouseDownListener::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	//subscribe or unsuscribe
	bool currFlag = inSolveInstance[0].GetBool();
	mButton = inSolveInstance[1].GetInt();
	if (currFlag && !isOn)
	{
		SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(Input_MouseDownListener, HandleMouseMove));
		isOn = true;
	}
	if (!currFlag && isOn)
	{
		UnsubscribeFromEvent(E_MOUSEMOVE);
		isOn = false;
	}

	outSolveInstance[0] = mPos;
	outSolveInstance[1] = mDelta;
}

void Input_MouseDownListener::HandleMouseMove(StringHash eventType, VariantMap& eventData)
{
	using namespace MouseMove;

	Input* input = GetSubsystem<Input>();
	if (input->GetMouseButtonDown(mButton))
	{
		IntVector2 pos = GetSubsystem<UI>()->GetCursorPosition();
		mPos = Vector3(pos.x_, pos.y_, 0);

		Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();
		//remap mouse pos by ui rect
		UIElement* element = (UIElement*)GetGlobalVar("activeUIRegion").GetPtr();


		if (element)
		{
			IntVector2 ePos = element->GetScreenPosition();
			IntVector2 eSize = element->GetSize();
			float x = (mPos.x_ - ePos.x_) / (float)eSize.x_;
			float y = (mPos.y_ - ePos.y_) / (float)eSize.y_;
			mPos = Vector3(x, y, 0);
		}
		else if (activeViewport)
		{
			IntRect viewRect = activeViewport->GetRect();
			mPos = Vector3((float)(pos.x_ - viewRect.left_) / viewRect.Width(),
				(float)(pos.y_ - viewRect.top_) / viewRect.Height(), 0);
		}


		int X = eventData[P_DX].GetInt();
		int Y = eventData[P_DY].GetInt();
		mDelta = Vector3(X, Y, 0);


		solvedFlag_ = 0;
		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}

}