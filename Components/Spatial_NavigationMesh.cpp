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


#include "Spatial_NavigationMesh.h"
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Navigation/Navigable.h>
#include <Urho3D/Navigation/NavigationMesh.h>
#include <Urho3D/Navigation/DynamicNavigationMesh.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Viewport.h>

#include "Polyline.h"


using namespace Urho3D;

String Spatial_NavigationMesh::iconTexture = "Textures/Icons/Spatial_NavigationMesh.png";

Spatial_NavigationMesh::Spatial_NavigationMesh(Context* context) : IoComponentBase(context, 0, 0)
{
	//set up component info
	SetName("AddComponent");
	SetFullName("Add Component");
	SetDescription("Adds a native component to a scene node.");

	//set up the slots
	AddInputSlot(
		"Node ID",
		"ID",
		"Node ID to add component",
		VAR_INT,
		DataAccess::LIST
	);

	AddInputSlot(
		"Padding",
		"P",
		"Padding",
		VAR_VECTOR3,
		DataAccess::ITEM,
		Vector3(0.0f, 10.0f, 0.0f)
	);

	AddInputSlot(
		"CellSize",
		"C",
		"Cell Size",
		VAR_FLOAT,
		DataAccess::ITEM,
		1.0f
	);

	AddInputSlot(
		"MaxSlope",
		"S",
		"Max Slope",
		VAR_FLOAT,
		DataAccess::ITEM,
		45.0f
	);

	AddOutputSlot(
		"Reference",
		"Ptr",
		"Reference to added component",
		VAR_PTR,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Geometry",
		"Geometry",
		"Polylines of nav mesh",
		VAR_VARIANTMAP,
		DataAccess::LIST
	);
}

void Spatial_NavigationMesh::PreLocalSolve()
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
void Spatial_NavigationMesh::SolveInstance(
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

	//add to scene root
	DynamicNavigationMesh* navMesh = scene->GetComponent<DynamicNavigationMesh>();
	if (!navMesh)
	{
		navMesh = scene->CreateComponent<DynamicNavigationMesh>();
		trackedItems.Push(navMesh->GetID());
	}

	//Navigable* navigable = scene->CreateComponent<Navigable>();
	//trackedItems.Push(navigable->GetID());

	VariantVector nodeList = inSolveInstance[0].GetVariantVector();
	if (nodeList.Empty() || nodeList[0].GetType() == VAR_NONE)
	{
		SetAllOutputsNull(outSolveInstance);
		scene->RemoveComponent<DynamicNavigationMesh>();
		return;
	}

	for (int i = 0; i < nodeList.Size(); i++)
	{
		//otherwise proceed with finding the node
		int nodeID = nodeList[i].GetInt();
		Node* node = scene->GetNode(nodeID);
		if (node)
		{
			Navigable* navigable = node->CreateComponent<Navigable>();
			//navigable->SetRecursive(true);
			trackedItems.Push(navigable->GetID());
		}
	}

	//settings
	Vector3 padding = inSolveInstance[1].GetVector3();
	float cellSize = inSolveInstance[2].GetFloat();
	float maxSlope = inSolveInstance[3].GetFloat();

	navMesh->SetPadding(padding);
	navMesh->SetAgentHeight(10.0f);
	navMesh->SetAgentRadius(1.0f);
	navMesh->SetCellHeight(0.05f);
	//navMesh->SetEdgeMaxLength(edgeLength);
	cellSize = Clamp(cellSize, 0.01f, 1000.0f);
	navMesh->SetCellSize(cellSize);
	navMesh->SetAgentMaxSlope(maxSlope);



	//build
	navMesh->Build();


	//export data
	Vector<Vector<Vector3>> polys;
	navMesh->GetNavMeshGeometry(polys);

	VariantVector polysOut;
	for (int i = 0; i < polys.Size(); i++)
	{
		Variant tmpPoly = Polyline_Make(polys[i]);
		Polyline_Close(tmpPoly);
		polysOut.Push(tmpPoly);
	}



	//output
	outSolveInstance[0] = navMesh;
	outSolveInstance[1] = polysOut;

}