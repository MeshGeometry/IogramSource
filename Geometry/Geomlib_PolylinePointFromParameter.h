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


#pragma once

#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Core/Variant.h>

// Inputs:
//   polyline: storing type VAR_VARIANTMAP, with map storing type VAR_VARIANTVECTOR under key "vertices", with those variants storing type VAR_VECTOR3;
//             >= 2 distinct points in the polyline
//   t:        0 <= t <= 1 (t will be set to an appropriate endpoint if it's outside the valid interval
// Outputs:
//   point: 3D point on the line corresponding to parameter t; start of polyline is t=0, end of polyline is t=1
// Returns true if computation was successfully carried out.
namespace Geomlib {
	bool PolylinePointFromParameter(
		const Urho3D::Variant& polyline,
		float t,
		Urho3D::Vector3& point
	);

	bool PolylineTransformFromParameter(
		const Urho3D::Variant& polyline,
		float t,
		Urho3D::Matrix3x4& transform
		);

	bool GetVertexNormal(const Urho3D::Variant& polyline, Urho3D::Vector3& normal, int vert_id);

}