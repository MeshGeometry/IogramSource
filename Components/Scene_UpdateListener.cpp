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


#include "Scene_UpdateListener.h"

#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Scene/Scene.h>
#include "IoGraph.h"

using namespace Urho3D;

String Scene_UpdateListener::iconTexture = "Textures/Icons/Scene_UpdateListener.png";

Scene_UpdateListener::Scene_UpdateListener(Urho3D::Context* context) : IoComponentBase(context, 1, 2)
{
	SetName("UpdateListener");
	SetFullName("Update Listener");
	SetDescription("Listens for scene updates");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("");

	inputSlots_[0]->SetName("Mute");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mute the listener");
	inputSlots_[0]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(true);
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("ElapsedTime");
	outputSlots_[0]->SetVariableName("ET");
	outputSlots_[0]->SetDescription("Elapsed Time since last start");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT); 
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("DeltaTime");
	outputSlots_[1]->SetVariableName("DT");
	outputSlots_[1]->SetDescription("Frame update time");
	outputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Scene_UpdateListener::PreLocalSolve()
{

}

void Scene_UpdateListener::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	//subscribe or unsuscribe
	bool currFlag = inSolveInstance[0].GetBool();
	if (!currFlag && isMuted)
	{
		SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(Scene_UpdateListener, HandleSceneUpdate));
		isMuted = false;
	}
	if (currFlag && !isMuted)
	{
		UnsubscribeFromEvent(E_SCENEUPDATE);
		isMuted = true;
	}
	
	
	outSolveInstance[0] = elapsedTime;
	outSolveInstance[1] = deltaTime;
}

void Scene_UpdateListener::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace SceneUpdate;
	elapsedTime = GetSubsystem<Time>()->GetElapsedTime();
	deltaTime = eventData[P_TIMESTEP].GetFloat();

	solvedFlag_ = 0;
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();

}