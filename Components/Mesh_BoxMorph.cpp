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


#include "Mesh_BoxMorph.h"

#include "IoGraph.h"

#include "Geomlib_ConstructTransform.h"
#include "ModelBoxMorph.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_BoxMorph::iconTexture = "Textures/Icons/Mesh_BoxMorph.png";

Mesh_BoxMorph::Mesh_BoxMorph(Urho3D::Context* context) : IoComponentBase(context, 1, 2)
{
	SetName("BoxMorph");
	SetFullName("Mesh Modeler");
	SetDescription("Mesh modeling through bounding box morph");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh to edit");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("EditedMesh");
	outputSlots_[0]->SetVariableName("EM");
	outputSlots_[0]->SetDescription("Edited mesh geometry");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("NodeID");
	outputSlots_[0]->SetVariableName("ID");
	outputSlots_[0]->SetDescription("NodeID");
	outputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	SubscribeToEvent("ModelEditChangeBoxMorph", URHO3D_HANDLER(Mesh_BoxMorph, HandleModelEdit));
}

void Mesh_BoxMorph::SolveInstance(
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

	//add new node
	String name = "BoxMorphModel";
	Node* newNode = scene->CreateChild(name);

	//create transform edit
	ModelBoxMorph* me = newNode->CreateComponent<ModelBoxMorph>();
	me->SetBaseGeometry(mesh);
	//me->SetEditing(render);


	//track
	trackedNodes.Push(newNode->GetID());

	outSolveInstance[0] = mesh;
	outSolveInstance[1] = newNode->GetID();
}

void Mesh_BoxMorph::PreLocalSolve()
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
				oldNode->RemoveAllChildren();
				oldNode->Remove();
			}

		}

		trackedNodes.Clear();
	}

}

void Mesh_BoxMorph::HandleModelEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{

	URHO3D_LOGINFO("Handling model change from inside graph component");

	VariantMap meshMap = eventData["EditedMesh"].GetVariantMap();
	outputSlots_[0]->SetIoDataTree(IoDataTree(GetContext(), meshMap));


	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}


