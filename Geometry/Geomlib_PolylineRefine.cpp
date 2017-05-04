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


#include "Geomlib_PolylineRefine.h"

#include <algorithm>
#include <vector>

#include "Geomlib_PolylinePointFromParameter.h"
#include "Polyline.h"

namespace { // woot got visual studio to stop indenting namespaces!

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

Vector<float> ComputePolylineParams(const Vector<Vector3>& vertexList)
{
	float length = 0.0f;
	// loop over each edge [i, i + 1]
	for (unsigned i = 0; i < vertexList.Size() - 1; ++i) {
		Vector3 seg = vertexList[i + 1] - vertexList[i];
		length += seg.Length();
	}
	if (Urho3D::Equals(length, 0.0f)) {
		// give up!
		return Vector<float>();
	}

	Vector<float> dists;

	float sublength = 0.0f;
	dists.Push(sublength);
	for (unsigned i = 0; i < vertexList.Size() - 1; ++i) {
		Vector3 seg = vertexList[i + 1] - vertexList[i];
		sublength += seg.Length();
		dists.Push(sublength);
	}

	Vector<float> ret;
	for (unsigned i = 0; i < dists.Size(); ++i) {
		ret.Push(dists[i] / length);
	}
	return ret;
}

} // namespace

// Inputs
//   ts: Requires no consecutive repeats in the list.
//       Each value in strict range 0 < t < 1.
bool Geomlib::PolylineRefine(
	const Urho3D::Variant& polylineIn,
	const Urho3D::Vector<float>& ts,
	Urho3D::Variant& polylineOut
)
{
	using Urho3D::Vector;
	using Urho3D::Vector3;
	using Urho3D::Variant;
	using Urho3D::VariantMap;
	using Urho3D::VariantVector;
	using Urho3D::Equals;

	VariantMap polyMap = polylineIn.GetVariantMap();
	VariantVector vertices = polyMap["vertices"].GetVariantVector();
	Vector<Vector3> vertexList;
	for (unsigned i = 0; i < vertices.Size(); ++i) {
		vertexList.Push(vertices[i].GetVector3());
	}
	if (vertexList.Size() <= 1) {
		polylineOut = polylineIn;
		return true;
	}

	// If there are no values there is nothing to do.
	if (ts.Size() == 0) {
		polylineOut = polylineIn;
		return true;
	}

	// Sort values and ensure all in the required range 0 <= t <= 1.
	Vector<float> vals = ts;
	Urho3D::Sort(vals.Begin(), vals.End());
	if (!(0.0f <= vals[0])) {
		polylineOut = Variant();
		return false;
	}
	if (!(vals[vals.Size() - 1] <= 1.0f)) {
		polylineOut = Variant();
		return false;
	}
	// Require no consecutive repeats in float value list.
	for (unsigned i = 0; i < vals.Size() - 1; ++i) {
		if (Equals(vals[i], vals[i + 1])) {
			polylineOut = Variant();
			return false;
		}
		if (!(vals[i] < vals[i + 1])) {
			polylineOut = Variant();
			return false;
		}
	}

	// Require no consecutive repeats in vertex list.
	for (unsigned i = 0; i < vertexList.Size() - 1; ++i) {
		if (Vector3Equals(vertexList[i], vertexList[i + 1])) {
			polylineOut = Variant();
			return false;
		}
	}
	// Require at least 2 vertices in polyline
	if (vertexList.Size() <= 1) {
		polylineOut = Variant();
		return false;
	}

	Vector<float> merged_vals = ComputePolylineParams(vertexList);
	assert(merged_vals.Size() > 0);
	assert(Equals(merged_vals[0], 0.0f));
	assert(Equals(merged_vals[merged_vals.Size() - 1], 1.0f));

	merged_vals.Push(ts);
	Urho3D::Sort(merged_vals.Begin(), merged_vals.End());
	Vector<float> clean_vals;
	clean_vals.Push(merged_vals[0]);
	for (unsigned i = 1; i < merged_vals.Size(); ++i) {
		if (Equals(merged_vals[i - 1], merged_vals[i])) {
			// Value repeats previous, do not record.
		}
		else {
			clean_vals.Push(merged_vals[i]);
		}
	}

	// clean_vals is ready
	VariantVector newVertexList;
	for (unsigned i = 0; i < clean_vals.Size(); ++i) {
		float par = clean_vals[i];
		Vector3 pt;
		PolylinePointFromParameter(polylineIn, par, pt);
		newVertexList.Push(Variant(pt));
	}

	//VariantMap outMap;
	//outMap["vertices"] = newVertexList;
	//polylineOut = Variant(outMap);

	polylineOut = Polyline_Make(newVertexList);

	return true;
}