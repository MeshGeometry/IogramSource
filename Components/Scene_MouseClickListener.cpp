#include "Scene_MouseClickListener.h"

#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "IoGraph.h"

#include "Urho3D/Physics/PhysicsWorld.h"

using namespace Urho3D;

String Scene_MouseClickListener::iconTexture = "Textures/Icons/Input_MouseClickListener.png";

Scene_MouseClickListener::Scene_MouseClickListener(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("MouseClickListener");
	SetFullName("Mouse Click Listener");
	SetDescription("Listens for mouse clickkinput");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("INPUT");

	inputSlots_[0]->SetName("On");
	inputSlots_[0]->SetVariableName("On");
	inputSlots_[0]->SetDescription("Listen for mouse clicks (bool)");
	inputSlots_[0]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(true);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Mouse button");
	inputSlots_[1]->SetVariableName("MB");
	inputSlots_[1]->SetDescription("Mouse Button");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(0);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Mouse Position");
	outputSlots_[0]->SetVariableName("MP");
	outputSlots_[0]->SetDescription("Mouse Position");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}


void Scene_MouseClickListener::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	//subscribe or unsuscribe
	bool currFlag = inSolveInstance[0].GetBool();
	if (currFlag && !isOn)
	{
		SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(Scene_MouseClickListener, HandleMouseClick));
		isOn = true;
	}
	if (!currFlag && isOn)
	{
		UnsubscribeFromEvent(E_MOUSEBUTTONDOWN);
		isOn = false;
	}

	outSolveInstance[0] = mPos;
}

void Scene_MouseClickListener::HandleMouseClick(StringHash eventType, VariantMap& eventData)
{
	using namespace MouseButtonDown;
	
	IntVector2 pos = GetSubsystem<Input>()->GetMousePosition();
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
		mPos = Vector3((float)(pos.x_ - viewRect.left_)/viewRect.Width(), 
			(float)(pos.y_ - viewRect.top_)/viewRect.Height(), 0);
	}

	solvedFlag_ = 0;
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}


