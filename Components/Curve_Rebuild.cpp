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


#include "Curve_Rebuild.h"
#include "Polyline.h"
#include <assert.h>

#include "Geomlib_RebuildPolyline.h"

using namespace Urho3D;

String Curve_Rebuild::iconTexture = "Textures/Icons/Curve_RebuildPolyline.png";

Curve_Rebuild::Curve_Rebuild(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("RefinePolyline");
	SetFullName("Refine Polyline");
	SetDescription("Refine polyline based on list of parameters");

	AddInputSlot(
		"Curve",
		"C",
		"Rebuilds the curve with the given number of segments",
		VAR_VARIANTMAP,
		DataAccess::ITEM
	);

	AddInputSlot(
		"NumPoints",
		"N",
		"Number of target points",
		VAR_INT,
		DataAccess::ITEM,
		10
	);

	AddOutputSlot(
		"Curve",
		"C",
		"The rebuilt curve.",
		VAR_VARIANTMAP,
		DataAccess::ITEM
	);
}

void Curve_Rebuild::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	if (!Polyline_Verify(inSolveInstance[0]))
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	int numPts = inSolveInstance[1].GetInt();

	Variant newPoly = Geomlib::RebuildPolyline(inSolveInstance[0], numPts);

	outSolveInstance[0] = newPoly;
}