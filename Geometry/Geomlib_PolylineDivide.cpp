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


#include "Geomlib_PolylineDivide.h"

#include <algorithm>
#include <vector>

#include "Geomlib_PolylinePointFromParameter.h"
#include "Polyline.h"

namespace {

using Urho3D::Vector;
using Urho3D::Variant;
using Urho3D::Vector3;
using Urho3D::Equals;

bool Vector3Equals(const Vector3& lhs, const Vector3& rhs)
{
	return Equals(lhs.x_, rhs.x_) && Equals(lhs.y_, rhs.y_) && Equals(lhs.z_, rhs.z_);
}

Vector<Vector3> RemoveRepeats(const Vector<Vector3>& vertexList)
{
	if (vertexList.Size() <= 1) {
		return vertexList;
	}

	Vector<Vector3> newVertexList;
	newVertexList.Push(vertexList[0]);
	for (unsigned i = 0; i < vertexList.Size() - 1; ++i) {
		if (!Vector3Equals(vertexList[i], vertexList[i + 1])) {
			newVertexList.Push(vertexList[i + 1]);
		}
	}

	return newVertexList;
}

} // namespace

bool Geomlib::PolylineDivide(
	const Urho3D::Variant& polylineIn,
	int n,
	Urho3D::Variant& polylineOut
)
{
	if (n <= 0) {
		polylineOut = Variant();
		return false;
	}
	if (!Polyline_Verify(polylineIn)) {
		polylineOut = Variant();
		return false;
	}

	float inc = 1.0f / n;

	Vector<float> params;
	for (unsigned i = 0; i < n; ++i) {
		params.Push(i * inc);
	}
	params.Push(1.0f);

	if (params.Size() <= 1) {
		polylineOut = Variant();
		return false;
	}

	Urho3D::VariantVector vertexList;
	for (unsigned i = 0; i < params.Size(); ++i) {
		Vector3 vec;
		float t = params[i];
		bool success = PolylinePointFromParameter(polylineIn, t, vec);
		if (!success) {
			polylineOut = Variant();
			return false;
		}
		vertexList.Push(Variant(vec));
	}
	if (vertexList.Size() <= 1) {
		polylineOut = Variant();
		return false;
	}

	polylineOut = Polyline_Make(vertexList);

	return true;
}
