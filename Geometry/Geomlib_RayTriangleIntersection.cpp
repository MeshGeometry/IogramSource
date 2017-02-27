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


#include "Geomlib_RayTriangleIntersection.h"

#include <Urho3D/Math/Vector3.h>

bool Geomlib::RayTriangleIntersection(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& C,
	const Urho3D::Vector3& O,
	const Urho3D::Vector3& D,
	float& s
)
{
	Urho3D::Vector3 E1 = B - A;
	Urho3D::Vector3 E2 = C - A;
	Urho3D::Vector3 P = D.CrossProduct(E2);
	float det = E1.DotProduct(P);

	if (det > -EPSILON && det < EPSILON) {
		return false;
	}

	float detInv = 1.0f / det;

	Urho3D::Vector3 T = O - A;
	float u = T.DotProduct(P) * detInv;
	if (u < 0.0f || u > 1.0f) {
		return false;
	}

	Urho3D::Vector3 Q = T.CrossProduct(E1);
	float v = D.DotProduct(Q) * detInv;
	if (v < 0.0f || u + v > 1.0f) {
		return false;
	}

	float t = E2.DotProduct(Q) * detInv;

	if (t > EPSILON) { // ray intersects triangle
		s = t;
		return true;
	}

	// ray misses triangle
	return false;
}