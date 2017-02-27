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


#include "Scene_Raycast.h"
#include <Urho3D/Graphics/Octree.h>
#include "Urho3D/Graphics/BillboardSet.h"

using namespace Urho3D;

String Scene_Raycast::iconTexture = "Textures/Icons/Scene_Raycast.png";

Scene_Raycast::Scene_Raycast(Urho3D::Context* context) : IoComponentBase(context, 4, 4)
{
	SetName("Raycast");
	SetFullName("Screen Raycast");
	SetDescription("Listens for screen raycasts, returns hit information");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("INPUT");


	inputSlots_[0]->SetName("StartPoint");
	inputSlots_[0]->SetVariableName("SP");
	inputSlots_[0]->SetDescription("Point in world coords");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Direction");
	inputSlots_[1]->SetVariableName("DR");
	inputSlots_[1]->SetDescription("Direction in which to cast ray");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[2]->SetName("Max Distance");
	inputSlots_[2]->SetVariableName("MD");
	inputSlots_[2]->SetDescription("Max distance to raycast");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(1000.0f);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("Mask");
	inputSlots_[3]->SetVariableName("M");
	inputSlots_[3]->SetDescription("Mask elements from raycast");
	inputSlots_[3]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->DefaultSet();

	outputSlots_[0]->SetName("Hit Points");
	outputSlots_[0]->SetVariableName("HP");
	outputSlots_[0]->SetDescription("Hit Points of ray");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Distance");
	outputSlots_[1]->SetVariableName("D");
	outputSlots_[1]->SetDescription("Distance");
	outputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[2]->SetName("Normal");
	outputSlots_[2]->SetVariableName("N");
	outputSlots_[2]->SetDescription("Normal");
	outputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[2]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[3]->SetName("ModelPtr");
	outputSlots_[3]->SetVariableName("MD");
	outputSlots_[3]->SetDescription("Model pointer.");
	outputSlots_[3]->SetVariantType(VariantType::VAR_PTR);
	outputSlots_[3]->SetDataAccess(DataAccess::ITEM);
}

void Scene_Raycast::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();

	Vector3 o = inSolveInstance[0].GetVector3();
	Vector3 d = inSolveInstance[1].GetVector3();
	float maxD = inSolveInstance[2].GetFloat();

	Ray ray = Ray(o, d);
	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, ray, RAY_TRIANGLE, maxD, DRAWABLE_GEOMETRY);

	//HACK! turn off any billboards in the scene
	PODVector<BillboardSet*> comps;
	scene->GetComponents<BillboardSet>(comps, true);

	for (int i = 0; i < comps.Size(); i++)
		comps[i]->SetEnabled(false);

	scene->GetComponent<Octree>()->RaycastSingle(query);

	if (results.Size() > 0)
	{
		outSolveInstance[0] = results[0].position_;
		outSolveInstance[1] = results[0].distance_;
		outSolveInstance[2] = results[0].normal_;
		outSolveInstance[3] = results[0].drawable_;
	}

	//turn points back on
	for (int i = 0; i < comps.Size(); i++)
		comps[i]->SetEnabled(true);
}