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


#include "Mesh_LinearDeformation.h"

#include "IoGraph.h"

#include "Geomlib_ConstructTransform.h"
#include "ModelEditLinear.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_LinearDeformation::iconTexture = "Textures/Icons/Mesh_LinearDeformation.png";

Mesh_LinearDeformation::Mesh_LinearDeformation(Urho3D::Context* context) : IoComponentBase(context, 3, 2)
{
	SetName("LinearDeformation");
	SetFullName("Mesh Modeler");
	SetDescription("Mesh modeling through Linear deformation");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh to edit");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Radius");
	inputSlots_[1]->SetVariableName("R");
	inputSlots_[1]->SetDescription("Radius of influence");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(1.0f);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("DisplayOn");
	inputSlots_[2]->SetVariableName("D");
	inputSlots_[2]->SetDescription("Toggle render on/off");
	inputSlots_[2]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(true);
	inputSlots_[2]->DefaultSet();



	outputSlots_[0]->SetName("EditedMesh");
	outputSlots_[0]->SetVariableName("EM");
	outputSlots_[0]->SetDescription("Edited mesh geometry");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("NodeID");
	outputSlots_[1]->SetVariableName("ID");
	outputSlots_[1]->SetDescription("Editor id");
	outputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	SubscribeToEvent("ModelEditChangeLinear", URHO3D_HANDLER(Mesh_LinearDeformation, HandleModelEdit));
}


void Mesh_LinearDeformation::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();

	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered.");
		return;
	}

	//get data and verify
	Variant mesh = inSolveInstance[0];
	if (!TriMesh_Verify(mesh)) {
		URHO3D_LOGWARNING("M1 must be a TriMesh!");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	if (inSolveInstance[1].GetType() != VAR_FLOAT && inSolveInstance[1].GetType() != VAR_INT) {
		URHO3D_LOGWARNING("R must be a number!");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	float radius = inSolveInstance[1].GetFloat();

	if (inSolveInstance[2].GetType() != VAR_BOOL) {
		URHO3D_LOGWARNING("D must be a boolean!");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	bool render = inSolveInstance[2].GetBool();


	//add new node
	Node* newNode = scene->CreateChild("ModelLinearEdit");

	//create transform edit
	ModelEditLinear* me = newNode->CreateComponent<ModelEditLinear>();
	me->SetRadius(radius);
	me->SetBaseGeometry(mesh);

	//me->SetEditing(render);


	//track
	trackedNodes.Push(newNode->GetID());


	outSolveInstance[0] = mesh;
	outSolveInstance[1] = newNode->GetID();
}

void Mesh_LinearDeformation::PreLocalSolve()
{
	//delete old nodes
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedNodes.Size(); i++)
		{
			Node* oldNode = scene->GetNode(trackedNodes[i]);

			if (oldNode)
			{
				//oldNode->RemoveAllChildren();
				oldNode->Remove();
			}

		}

		trackedNodes.Clear();
	}

}

void Mesh_LinearDeformation::HandleModelEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{

	URHO3D_LOGINFO("Handling model change from inside graph component");

	VariantMap meshMap = eventData["EditedMesh"].GetVariantMap();
	outputSlots_[0]->SetIoDataTree(IoDataTree(GetContext(), meshMap));


	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}


