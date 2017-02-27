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


#include "Vector_BestFitPlane.h"

#include <assert.h>

#include <iostream>

#include <Urho3D/Core/Variant.h>

#include "TriMesh.h"
#include "Polyline.h"

#include "Geomlib_BestFitPlane.h"

using namespace Urho3D;

String Vector_BestFitPlane::iconTexture = "Textures/Icons/Vector_BestFitPlane.png";

Vector_BestFitPlane::Vector_BestFitPlane(Context* context) : IoComponentBase(context, 1, 2)
{
	SetName("BestFitPlane");
	SetFullName("Best-Fit Plane");
	SetDescription("Compute the least-squares plane of best fit");
	SetGroup(IoComponentGroup::VECTOR);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Geometry");
	inputSlots_[0]->SetVariableName("G");
	inputSlots_[0]->SetDescription("Geometry determining the point cloud");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("Point");
	outputSlots_[0]->SetVariableName("P");
	outputSlots_[0]->SetDescription("Reference point on best-fit plane");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Normal");
	outputSlots_[1]->SetVariableName("N");
	outputSlots_[1]->SetDescription("Normal to best-fit plane");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Vector_BestFitPlane::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	Variant geo = inSolveInstance[0];
	Vector<Vector3> point_cloud;
	if (Polyline_Verify(geo)) {
		point_cloud = Polyline_ComputePointCloud(geo);
	}
	else if (TriMesh_Verify(geo)) {
		point_cloud = TriMesh_ComputePointCloud(geo);
	}
	else {
		URHO3D_LOGWARNING("G must be a triangle mesh or polyline.");
		outSolveInstance[0] = Variant();
		outSolveInstance[1] = Variant();
		return;
	}

	if (point_cloud.Size() <= 0) {
		URHO3D_LOGWARNING("G must generate a non-empty point cloud.");
		outSolveInstance[0] = Variant();
		outSolveInstance[1] = Variant();
		return;
	}

	//

	///////////////////
	// COMPONENT'S WORK

	Vector3 point, normal;
	Geomlib::BestFitPlane(point_cloud, point, normal);

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = point;
	outSolveInstance[1] = normal;
}