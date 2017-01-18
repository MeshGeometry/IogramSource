#include "Scene_HandleEvent.h"

#include <assert.h>
#include "IoGraph.h"

using namespace Urho3D;

String Scene_HandleEvent::iconTexture = "Textures/Icons/Scene_HandleEvent.png";

Scene_HandleEvent::Scene_HandleEvent(Context* context) :
	IoComponentBase(context, 1, 1)
{
	SetName("HandleEvent");
	SetFullName("Handle Event");
	SetDescription("Receive an Event");

	inputSlots_[0]->SetName("EventName");
	inputSlots_[0]->SetVariableName("E");
	inputSlots_[0]->SetDescription("Unique event name to send");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("DataTree");
	outputSlots_[0]->SetVariableName("T");
	outputSlots_[0]->SetDescription("DataTree to send");
	outputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}


int Scene_HandleEvent::LocalSolve()
{
	assert(inputSlots_.Size() >= 1);
	assert(outputSlots_.Size() >= 1);

	if (inputSlots_[0]->HasNoData()) {
		solvedFlag_ = 0;
		return 0;
	}

	IoDataTree nameTree = inputSlots_[0]->GetIoDataTree();

	Vector<int> path = nameTree.Begin();
	Variant name;
	nameTree.GetItem(name, path, 0);

	String eventName = name.GetString();
	if (eventName.Empty())
	{
		solvedFlag_ = 0;
		return 0;
	}

	
	SubscribeToEvent(eventName, URHO3D_HANDLER(Scene_HandleEvent, GenericEventHandler));

	solvedFlag_ = 1;
	return 1;
}


void Scene_HandleEvent::GenericEventHandler(StringHash eventType, VariantMap& eventData)
{
	IoDataTree* dataTree = (IoDataTree*)eventData["DataTree"].GetPtr();

	if (dataTree)
	{
		outputSlots_[0]->SetIoDataTree(*dataTree);
	}

	solvedFlag_ = 1;
	return;
}