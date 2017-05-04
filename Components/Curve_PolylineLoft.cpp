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


#include "Curve_PolylineLoft.h"

#include <assert.h>

#include "Geomlib_PolylineLoft.h"
#include "Polyline.h"

using namespace Urho3D;

String Curve_PolylineLoft::iconTexture = "Textures/Icons/Curve_PolylineLoft.png";

Curve_PolylineLoft::Curve_PolylineLoft(Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("LoftPolylines");
	SetFullName("Loft Polylines");
	SetDescription("Perform loft operation on a list of polylines");
	SetGroup(IoComponentGroup::CURVE);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("PolylineList");
	inputSlots_[0]->SetVariableName("L");
	inputSlots_[0]->SetDescription("List of polylines to loft");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Lofted mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_PolylineLoft::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	if (inSolveInstance[0].GetType() != VariantType::VAR_VARIANTVECTOR) {
		URHO3D_LOGWARNING("L must be a list of polylines.");
		outSolveInstance[0] = Variant();
		return;
	}
	VariantVector polyList = inSolveInstance[0].GetVariantVector();
	for (unsigned i = 0; i < polyList.Size(); ++i) {
		if (!Polyline_Verify(polyList[i])) {
			URHO3D_LOGWARNING("L must contain only valid polylines.");
			outSolveInstance[0] = Variant();
			return;
		}
	}

	///////////////////
	// COMPONENT'S WORK

	Variant mesh;
	bool success = Geomlib::PolylineLoft(polyList, mesh);
	if (!success) {
		URHO3D_LOGWARNING("PolylineLoft operation failed.");
		outSolveInstance[0] = Variant();
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = mesh;
}