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


#include "Curve_Polygon.h"

#include <assert.h>


#include "StockGeometries.h"

#include "Geomlib_TransformVertexList.h"

#include "Polyline.h"

using namespace Urho3D;

String Curve_Polygon::iconTexture = "Textures/Icons/Curve_Polygon.png";

Curve_Polygon::Curve_Polygon(Context* context) :
	IoComponentBase(context, 1, 2)
{
	SetName("Polygon");
	SetFullName("Construct Polygon");
	SetDescription("Construct a polygon with n sides");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("Number of sides");
	inputSlots_[0]->SetVariableName("N");
	inputSlots_[0]->SetDescription("Number of sides for the polygon");
	inputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant(5));
	inputSlots_[0]->DefaultSet();

//	inputSlots_[1]->SetName("Transformation");
//	inputSlots_[1]->SetVariableName("T");
//	inputSlots_[1]->SetDescription("Transformation to apply to polygon");
//	inputSlots_[1]->SetVariantType(VariantType::VAR_MATRIX3X4);
//	inputSlots_[1]->SetDefaultValue(Matrix3x4::IDENTITY);
//	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Polygon");
	outputSlots_[0]->SetVariableName("P");
	outputSlots_[0]->SetDescription("Constructed polygon");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Vertices");
	outputSlots_[1]->SetVariableName("V");
	outputSlots_[1]->SetDescription("Constructed vertices");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[1]->SetDataAccess(DataAccess::LIST);
}

void Curve_Polygon::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	// EXTRACT & VERIFY
	if (!IsAllInputValid(inSolveInstance)) {
		SetAllOutputsNull(outSolveInstance);
	}
	int n = inSolveInstance[0].GetInt();
    Urho3D::Matrix3x4 tr = Matrix3x4::IDENTITY;
	if (n <= 2) {
		URHO3D_LOGWARNING("N must be >= 3");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	// COMPONENT'S WORK

	Variant base_polygon = MakeRegularPolygon(n);
	VariantVector base_vertex_list = Polyline_ComputeSequentialVertexList(base_polygon);
	VariantVector vertex_list = Geomlib::TransformVertexList(tr, base_vertex_list);
	Variant polygon = Polyline_Make(vertex_list);

	outSolveInstance[0] = polygon;
	outSolveInstance[1] = vertex_list;
}
