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


#include "Geomlib_RebuildPolyline.h"
#include "Geomlib_PolylinePointFromParameter.h"
#include "Polyline.h"

using namespace Urho3D;

Urho3D::Variant Geomlib::RebuildPolyline(
	const Urho3D::Variant& polylineIn,
	int numSegments
)
{
	bool ver = Polyline_Verify(polylineIn);
	if (!ver)
	{
		return false;
	}

	bool closed = Polyline_IsClosed(polylineIn);
	VariantVector newVerts;

	//push the first point
	Vector3 pt;
	for (int i = 0; i <= numSegments; i++)
	{
		float t = (float)i / (float)numSegments;
		PolylinePointFromParameter(polylineIn, t, pt);
		newVerts.Push(pt);
	}

	return Polyline_Make(newVerts);
}