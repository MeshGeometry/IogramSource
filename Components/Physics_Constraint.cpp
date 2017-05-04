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


#include "Physics_Constraint.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Physics/Constraint.h>
#include <Urho3D/Physics/RigidBody.h>

using namespace Urho3D;

String Physics_Constraint::iconTexture = "Textures/Icons/Physics_Constraint.png";

void Physics_Constraint::PreLocalSolve()
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


Physics_Constraint::Physics_Constraint(Urho3D::Context* context) : IoComponentBase(context, 5, 1)
{
	SetName("PhysicsConstraint");
	SetFullName("Physics Constraint");
	SetDescription("Construct a constraint between a rigidbody and an optional second one.");

	inputSlots_[0]->SetName("Body A");
	inputSlots_[0]->SetVariableName("A");
	inputSlots_[0]->SetDescription("First rigidbody.");
	inputSlots_[0]->SetVariantType(VariantType::VAR_PTR);

	inputSlots_[1]->SetName("Local Anchor A");
	inputSlots_[1]->SetVariableName("X");
	inputSlots_[1]->SetDescription("Position in local coordinates for start of constraint");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDefaultValue(Vector3::ZERO);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Body B");
	inputSlots_[2]->SetVariableName("B");
	inputSlots_[2]->SetDescription("Second rigidbody.");
	inputSlots_[2]->SetVariantType(VariantType::VAR_PTR);

	inputSlots_[3]->SetName("Local Anchor B");
	inputSlots_[3]->SetVariableName("Y");
	inputSlots_[3]->SetDescription("Position in local coordinates for end of constraint");
	inputSlots_[3]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[3]->SetDefaultValue(Vector3::ZERO);
	inputSlots_[3]->DefaultSet();

	inputSlots_[4]->SetName("Constraint Type");
	inputSlots_[4]->SetVariableName("T");
	inputSlots_[4]->SetDescription("Type of constraint");
	inputSlots_[4]->SetVariantType(VariantType::VAR_STRING);		


	outputSlots_[0]->SetName("Constraint");
	outputSlots_[0]->SetVariableName("C");
	outputSlots_[0]->SetDescription("Constraint");
	outputSlots_[0]->SetVariantType(VariantType::VAR_PTR);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Physics_Constraint::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	RigidBody* startRB = (RigidBody*)inSolveInstance[0].GetPtr();
	RigidBody* endRB = (RigidBody*)inSolveInstance[2].GetPtr();

	if (!startRB) {
		URHO3D_LOGERROR("At least one valid rigid body is required.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Vector3 startPt = inSolveInstance[1].GetVector3();
	Vector3 endPt = inSolveInstance[3].GetVector3();
	String typeName = inSolveInstance[4].GetString();

	Node* node = startRB->GetNode();
	Constraint* constraint = node->CreateComponent<Constraint>();
	ConstraintType type = ConstraintType::CONSTRAINT_CONETWIST;
	constraint->SetConstraintType(type);
	constraint->SetPosition(startPt);
	constraint->SetOtherPosition(endPt);
	constraint->SetOtherBody(endRB);

	
	trackedItems.Push(constraint->GetID());
	outSolveInstance[0] = constraint;

}