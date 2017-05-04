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


#include "Physics_ApplyForce.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Physics/RigidBody.h>

using namespace Urho3D;

String Physics_ApplyForce::iconTexture = "Textures/Icons/Physics_ApplyForce.png";


Physics_ApplyForce::Physics_ApplyForce(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("ApplyForce");
	SetFullName("Apply Force");
	SetDescription("Apply a force (vector with magnitude) to a rigid body");

	inputSlots_[0]->SetName("RigidBody");
	inputSlots_[0]->SetVariableName("R");
	inputSlots_[0]->SetDescription("RigidyBody");
	inputSlots_[0]->SetVariantType(VariantType::VAR_PTR);


	inputSlots_[1]->SetName("Force");
	inputSlots_[1]->SetVariableName("F");
	inputSlots_[1]->SetDescription("Vector defining the force");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->DefaultSet();


	outputSlots_[0]->SetName("RigidBody");
	outputSlots_[0]->SetVariableName("R");
	outputSlots_[0]->SetDescription("Rigid Body");
	outputSlots_[0]->SetVariantType(VariantType::VAR_PTR);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Physics_ApplyForce::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{

	RigidBody* rb = (RigidBody*)inSolveInstance[0].GetPtr();

	if (!rb) {
		URHO3D_LOGERROR("At least one valid rigid body is required.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Vector3 force = inSolveInstance[1].GetVector3();
	rb->ApplyForce(force);

	outSolveInstance[0] = rb;
}