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


#include "Spatial_CrowdAgent.h"
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Navigation/Navigable.h>
#include <Urho3D/Navigation/DynamicNavigationMesh.h>
#include <Urho3D/Navigation/CrowdManager.h>
#include <Urho3D/Navigation/CrowdAgent.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>


#include "Polyline.h"


using namespace Urho3D;

String Spatial_CrowdAgent::iconTexture = "Textures/Icons/Spatial_CrowdAgent.png";

Spatial_CrowdAgent::Spatial_CrowdAgent(Context* context) : IoComponentBase(context, 0, 0)
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
		"CrowdManager",
		"CM",
		"Crowd Manager",
		VAR_PTR,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Start",
		"S",
		"Start Point",
		VAR_VECTOR3,
		DataAccess::ITEM,
		Vector3(0, 0, 0)
	);

	AddInputSlot(
		"Target",
		"T",
		"Target Point",
		VAR_VECTOR3,
		DataAccess::ITEM,
		Vector3(100, 0, 100)
	);

	AddInputSlot(
		"Velocity",
		"V",
		"Velocity",
		VAR_FLOAT,
		DataAccess::ITEM,
		1.0f
	);

	AddInputSlot(
		"Agent Dimensions",
		"D",
		"Radius and height of agent.",
		VAR_VECTOR3,
		DataAccess::ITEM,
		Vector3(1.0f, 2.0f, 1.0f)
	);

	AddInputSlot(
		"Visualize",
		"VZ",
		"Visualize",
		VAR_BOOL,
		DataAccess::ITEM,
		true
	);

	AddOutputSlot(
		"NodeID",
		"ID",
		"Node ID",
		VAR_INT,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Reference",
		"Ptr",
		"Reference to added component",
		VAR_PTR,
		DataAccess::ITEM
	);
}

void Spatial_CrowdAgent::PreLocalSolve()
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedItems.Size(); i++)
		{
			Node* sm = scene->GetNode(trackedItems[i]);
			if (sm != NULL)
			{
				sm->Remove();
			}
		}
	}

	trackedItems.Clear();
}

//work function
void Spatial_CrowdAgent::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered!");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	DynamicNavigationMesh* nm = (DynamicNavigationMesh*)inSolveInstance[0].GetPtr();
	CrowdManager* cm = (CrowdManager*)inSolveInstance[1].GetPtr();
	if (!nm || !cm)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}


	Vector3 start = inSolveInstance[2].GetVector3();
	Vector3 end = inSolveInstance[3].GetVector3();
	float maxVel = inSolveInstance[4].GetFloat();
	maxVel = Clamp(maxVel, 0.0f, M_LARGE_VALUE);
	Vector3 dims = inSolveInstance[5].GetVector3();
	bool viz = inSolveInstance[6].GetBool();

	//load an instance of agent geometry
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	Node* agentViz = scene->CreateChild("NavAgent");

	if (viz) {
		StaticModel* model = agentViz->CreateComponent<StaticModel>();
		model->SetModel(cache->GetResource<Model>("Models/Agent.mdl"));
		Material* material = cache->GetResource<Material>("Materials/DefaultGrey.xml");
		model->SetMaterial(material);
		model->SetCastShadows(true);
	}


	//output
	Vector3 nearestNavPoint = nm->FindNearestPoint(start, nm->GetBoundingBox().Size());
	agentViz->SetPosition(nearestNavPoint);
	CrowdAgent* ca = agentViz->CreateComponent<CrowdAgent>();
	ca->SetRadius(dims.x_);
	ca->SetHeight(dims.y_);
	ca->SetNavigationQuality(NAVIGATIONQUALITY_HIGH);
	ca->SetMaxSpeed(maxVel);
	ca->SetMaxAccel(2.0f * maxVel);


	//set target
	Vector3 endTarget = nm->FindNearestPoint(end, nm->GetBoundingBox().Size());
	ca->SetTargetPosition(endTarget);
	

	trackedItems.Push(agentViz->GetID());

	outSolveInstance[0] = agentViz->GetID();
	outSolveInstance[1] = ca;


}