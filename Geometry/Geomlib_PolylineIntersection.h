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

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>

namespace Geomlib {

	// [t1, t2] interval of real number line; no promise that t1 <= t2
	// [s1, s2] interval of real number line; no promise that s1 <= s2
	// Returns:
	//   1 if [t1, t2] and [s1, s2] have non-empty intersection
	//   0 otherwise
	int IntervalIntervalIntersection(
		double t1,
		double t2,
		double s1,
		double s2);

	//   1 if (A,B) and (C,D) have non-empty intersection
	//   0 otherwise
	int SegmentSegmentIntersection2D(
		Urho3D::Vector2& A,
		Urho3D::Vector2& B,
		Urho3D::Vector2& C,
		Urho3D::Vector2& D);

	int SegmentSegmentIntersection3D(
		Urho3D::Vector3& A,
		Urho3D::Vector3& B,
		Urho3D::Vector3& C,
		Urho3D::Vector3& D,
		Urho3D::Vector3& normal);

	// ONLY FOR PLANAR POLYLINES

	// returns true if the polyline has a self-intersection
	// does not count a closed polyline as self-intersecting
	bool HasSelfIntersection(const Urho3D::Variant polyline);

	// returns a list of self intersections of the polyline
	int GetSelfIntersections(
		const Urho3D::Variant polyline,
		Urho3D::Vector<Urho3D::Vector3>& intersections, 
		Urho3D::Variant& revised_polyline
	);

	// inserts the point into the vertex list between A, B and C, D 
	int InsertSelfIntersectionsAsPoint(
		const Urho3D::Vector3& A,
		const Urho3D::Vector3& B,
		const Urho3D::Vector3& C,
		const Urho3D::Vector3& D,
		const Urho3D::Vector3& point,
		Urho3D::Vector<Urho3D::Vector3>& verts);

	int GetPointOfIntersection(
		const Urho3D::Vector3& A,
		const Urho3D::Vector3& B,
		const Urho3D::Vector3& C,
		const Urho3D::Vector3& D, 
		Urho3D::Vector3& point);

	bool ShortestDistance(
		const Urho3D::Vector3& A,
		const Urho3D::Vector3& B,
		const Urho3D::Vector3& C,
		const Urho3D::Vector3& D,
		double& ta, double& tb);

	Urho3D::Variant Polyline_Make_With_Intersections(const Urho3D::Vector<Urho3D::Vector3>& vertexList);


}