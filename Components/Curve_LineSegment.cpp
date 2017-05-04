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


#include "Curve_LineSegment.h"

#include <assert.h>

#include "Polyline.h"

using namespace Urho3D;

String Curve_LineSegment::iconTexture = "Textures/Icons/Curve_LineSegment.png";

Curve_LineSegment::Curve_LineSegment(Context* context) :
	IoComponentBase(context, 2, 1)
{
	SetName("LineSegment");
	SetFullName("Construct Line Segment");
	SetDescription("Construct a line segment from start and end vertices");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("StartVertex");
	inputSlots_[0]->SetVariableName("A");
	inputSlots_[0]->SetDescription("Start of segment");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Vector3::ZERO);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("EndVertex");
	inputSlots_[1]->SetVariableName("B");
	inputSlots_[1]->SetDescription("End of segment");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Vector3(10.0f, 10.0f, 10.0f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("LineSegment");
	outputSlots_[0]->SetVariableName("L");
	outputSlots_[0]->SetDescription("Constructed line segment");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_LineSegment::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slots 0 and 1 together
	if (inSolveInstance[0].GetType() != VAR_VECTOR3 || inSolveInstance[1].GetType() != VAR_VECTOR3)
	{
		URHO3D_LOGWARNING("A and B must both have type Vector3.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Vector3 startVert = inSolveInstance[0].GetVector3();
	Vector3 endVert = inSolveInstance[1].GetVector3();

	VariantMap polyline;
	VariantVector verts;

	verts.Push(startVert);
	verts.Push(endVert);

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = Polyline_Make(verts);
}