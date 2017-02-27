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


#include "Geomlib_PolylinePointToTransform.h"

#include "Polyline.h"

using Urho3D::Variant;
using Urho3D::VariantVector;
using Urho3D::VariantType;
using Urho3D::Vector3;
using Urho3D::VariantMap;
using Urho3D::VariantType;
using Urho3D::Quaternion;
using Urho3D::Matrix3x4;

bool Geomlib::PolylinePointToTransform(
	const Urho3D::Variant& polyline,
	int i,
	Urho3D::Variant& T
)
{
	// For now (?) needs to be the coords of vertices visited by the polygon, in sequence
	VariantVector verts;
	bool ver = Polyline_Verify(polyline);
	if (!ver) {
		if (polyline.GetType() != VariantType::VAR_VARIANTMAP) {
			T = Variant();
			return false;
		}
		VariantMap pMap = polyline.GetVariantMap();
		Variant vertVar = pMap["vertices"];
		if (vertVar.GetType() != VariantType::VAR_VARIANTVECTOR) {
			T = Variant();
			return false;
		}
		verts = vertVar.GetVariantVector();
	}
	else {
		verts = Polyline_ComputeSequentialVertexList(polyline);
	}

	// Check that i is in the correct range
	if (i < 0 || i > verts.Size() - 1) {
		T = Variant();
		return false;
	}

	// Check if there are enough vertices to make this operation meaningful
	if (verts.Size() == 0) {
		T = Variant();
		return false;
	}
	else if (verts.Size() == 1) {
		T = Variant();
		return false;
	}

	// verts.Size() >= 2

	// Handle endpoints
	if (i == 0) {
		Vector3 q = verts[0].GetVector3();
		Vector3 r = verts[1].GetVector3();
		Vector3 tangent = r - q;
		Vector3 n; // anything normal to tangent???
		// ...
		T = Variant(Matrix3x4::IDENTITY);
		return true;
	}
	else if (i == verts.Size() - 1) {
		// ...
		T = Variant(Matrix3x4::IDENTITY);
		return true;
	}

	// If verts.Size() == 2 then i must be either 0 or 1 (1 == verts.Size() - 1), so this if should never be entered
	if (verts.Size() == 2) {
		// TEMPORARY
		T = Variant();
		return false;
	}

	// verts.Size() >= 3 and 0 <= i <= verts.Size() - 1 indexes a strictly interior vertex

	Vector3 p = verts[i - 1].GetVector3();
	Vector3 q = verts[i].GetVector3();
	Vector3 r = verts[i + 1].GetVector3();

	Vector3 v = q - p;
	Vector3 w = r - q;

	Vector3 tangent = 0.5f * (v + w); // weight by lengths?
	if (tangent.LengthSquared() <= 0.0f) {
		T = Variant(Matrix3x4(q, Quaternion::IDENTITY, 1.0f));
		return true;
	}

	Vector3 n = v.CrossProduct(w);

	if (n.LengthSquared() <= 0.0f) {
		Vector3 zAxis = tangent.Normalized();
		Vector3 yAxis; // this should just be anything normal to zAxis ???
		Vector3 xAxis = zAxis.CrossProduct(yAxis);
		// ...
		T = Variant();
		return false;
	}

	Vector3 zAxis = tangent.Normalized();
	Vector3 yAxis = n.Normalized();
	Vector3 xAxis = zAxis.CrossProduct(yAxis);

	Quaternion quat(xAxis, yAxis, zAxis);
	T = Variant(Matrix3x4(q, quat, 1.0f));
	return true;
}