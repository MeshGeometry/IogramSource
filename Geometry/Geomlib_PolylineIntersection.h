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

	//// From Mesh Repair:

	//// [t1, t2] interval of real number line; no promise that t1 <= t2
	//// [s1, s2] interval of real number line; no promise that s1 <= s2
	//// Returns:
	////   1 if [t1, t2] and [s1, s2] have non-empty intersection
	////   0 otherwise
	//int IntervalIntervalIntersection(
	//	double t1,
	//	double t2,
	//	double s1,
	//	double s2);

	//// returns	1 for intersection (including intersection at endpoints)
	////			0 for no intersection
	//int SegmentSegmentIntersection2D(
	//	const Eigen::RowVector2d& A,
	//	const Eigen::RowVector2d& B,
	//	const Eigen::RowVector2d& C,
	//	const Eigen::RowVector2d& D);

	//// returns	1 for intersection (including intersection at endpoints)
	////			0 for no intersection
	//int SegmentSegmentIntersection3D(
	//	const Eigen::RowVector3d& A,
	//	const Eigen::RowVector3d& B,
	//	const Eigen::RowVector3d& C,
	//	const Eigen::RowVector3d& D,
	//	NgonPlane facePlane);



	//// Computes the list of edges to check in IsFacePolySelfIntersecting, N = # of edges
	//// Note for edge 0, we only check it with edges 2, 3, 4, ..., N-2
	//// And we only check pairs of the form (A, B) where B>A (removes duplicate checks)
	//std::vector<std::vector<int>> GetEdgesToIntersect(int N);

}