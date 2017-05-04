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


#include "Spatial_CrowdManager.h"
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Navigation/Navigable.h>
#include <Urho3D/Navigation/DynamicNavigationMesh.h>
#include <Urho3D/Navigation/CrowdManager.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Viewport.h>

#include "Polyline.h"


using namespace Urho3D;

String Spatial_CrowdManager::iconTexture = "Textures/Icons/Spatial_CrowdManager.png";

Spatial_CrowdManager::Spatial_CrowdManager(Context* context) : IoComponentBase(context, 0, 0)
{
	//set up component info
	SetName("CrowdManager");
	SetFullName("Crowd Manager");
	SetDescription("Adds a crowd manager to scene root.");

	//set up the slots
	AddInputSlot(
		"NavMesh",
		"N",
		"Nav Mesh",
		VAR_PTR,
		DataAccess::ITEM
		);


	AddInputSlot(
		"Quality",
		"Q",
		"Quality level of the crowd",
		VAR_INT,
		DataAccess::ITEM,
		10
	);

	AddOutputSlot(
		"Reference",
		"Ptr",
		"Reference to added component",
		VAR_PTR,
		DataAccess::ITEM
	);
}

void Spatial_CrowdManager::PreLocalSolve()
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedItems.Size(); i++)
		{
			Component* sm = scene->GetComponent(trackedItems[i]);
			if (sm != NULL)
			{
				sm->Remove();
			}
		}
	}

	trackedItems.Clear();
}

//work function
void Spatial_CrowdManager::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered!");
		outSolveInstance[0] = Variant();
		return;
	}

	DynamicNavigationMesh* dm = (DynamicNavigationMesh*)inSolveInstance[0].GetPtr();
	if (!dm)
	{
		outSolveInstance[0] = Variant();
		return;
	}

	//add to scene root
	CrowdManager* crowdManager = scene->GetComponent<CrowdManager>();
	if (!crowdManager)
	{
		crowdManager = scene->CreateComponent<CrowdManager>();

		//set params
		CrowdObstacleAvoidanceParams params = crowdManager->GetObstacleAvoidanceParams(0);

		// Set the params to "High (66)" setting
		params.velBias = 0.5f;
		params.adaptiveDivs = 7;
		params.adaptiveRings = 3;
		params.adaptiveDepth = 3;
		crowdManager->SetObstacleAvoidanceParams(0, params);

		trackedItems.Push(crowdManager->GetID());

		crowdManager->SetNavigationMesh(dm);

	}

	//output
	outSolveInstance[0] = crowdManager;

}