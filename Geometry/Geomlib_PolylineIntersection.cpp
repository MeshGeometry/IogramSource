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

#include "Geomlib_PolylineIntersection.h""


// [t1, t2] interval of real number line; no promise that t1 <= t2
// [s1, s2] interval of real number line; no promise that s1 <= s2
// Returns:
//   1 if [t1, t2] and [s1, s2] have non-empty intersection
//   0 otherwise

//int IntervalIntervalIntersection(
//	double t1,
//	double t2,
//	double s1,
//	double s2)
//{
//	if (t1 > t2) {
//		double x = t1;
//		t1 = t2;
//		t2 = x;
//	}
//	if (s1 > s2) {
//		double y = s1;
//		s1 = s2;
//		s2 = y;
//	}
//
//	if (s1 <= t1 && t1 <= s2) {
//		return 1;
//	}
//	else if (s1 <= t2 && t2 <= s2) {
//		return 1;
//	}
//	else if (t1 <= s1 && s1 <= t2) {
//		return 1;
//	}
//
//	return 0;
//}
//
////   1 if (A,B) and (C,D) have non-empty intersection
////   0 otherwise
//int SegmentSegmentIntersection2D(
//	const Eigen::RowVector2d& A,
//	const Eigen::RowVector2d& B,
//	const Eigen::RowVector2d& C,
//	const Eigen::RowVector2d& D)
//{
//	// left & right half plane checks
//	if (
//		(A(0) > 0 && B(0) > 0) && // A,B in right half plane
//		(C(0) < 0 && D(0) < 0)    // C,D in left half plane
//		) {
//		return 0;
//	}
//	if (
//		(A(0) < 0 && B(0) < 0) && // A,B in left half plane
//		(C(0) > 0 && D(0) > 0)    // C,D in right half plane
//		) {
//		return 0;
//	}
//	// upper & lower half plane checks
//	if (
//		(A(1) > 0 && B(1) > 0) && // A,B in upper half plane
//		(C(1) < 0 && D(1) < 0)    // C,D in lower half plane
//		) {
//		return 0;
//	}
//	if (
//		(A(1) < 0 && B(1) < 0) && // A,B in lower half plane
//		(C(1) > 0 && D(1) > 0)    // C,D in upper half plane
//		) {
//		return 0;
//	}
//
//	// do C and D lie strictly on the same side of line through AB?
//	double cval = (B(1) - A(1)) * (C(0) - A(0)) + (A(0) - B(0)) * (C(1) - A(1));
//	double dval = (B(1) - A(1)) * (D(0) - A(0)) + (A(0) - B(0)) * (D(1) - A(1));
//	if ((cval > 0 && dval > 0) || (cval < 0 && dval < 0)) {
//		return 0;
//	}
//	// do A and B lie strictly on the same side of line through CD?
//	double aval = (D(1) - C(1)) * (A(0) - C(0)) + (C(0) - D(0)) * (A(1) - C(1));
//	double bval = (D(1) - C(1)) * (B(0) - C(0)) + (C(0) - D(0)) * (B(1) - C(1));
//	if ((aval > 0 && bval > 0) || (aval < 0 && bval < 0)) {
//		return 0;
//	}
//
//	// lines through AB and CD definitely intersect
//	double k11 = B(0) - A(0);
//	double k12 = C(0) - D(0);
//	double k21 = B(1) - A(1);
//	double k22 = C(1) - D(1);
//	double z1 = C(0) - A(0);
//	double z2 = C(1) - A(1);
//	double prod1 = k11 * k22;
//	double prod2 = k12 * k21;
//	double prod3 = k22 * z1;
//	double prod4 = k12 * z2;
//	if (abs(prod1) > 1e16 || abs(prod2) > 1e16 || abs(prod3) > 1e16 || abs(prod4) > 1e16) {
//		double shrink = 1e-6;
//		return SegmentSegmentIntersection2D(shrink * A, shrink * B, shrink * C, shrink * D);
//	}
//	double denom = prod1 - prod2;
//	double numer = prod3 - prod4;
//	if (prod1 == prod2) {
//		denom = 0;
//	}
//
//	const double eps = 1e-8;
//
//	if (denom > eps || -denom > eps) {
//		// there is unique point of intersection: does it lie in segment AB?
//		double t = (k22 * z1 - k12 * z2) / denom;
//		return 0 <= t && t <= 1;
//	}
//	else {
//		// lines through AB and CD coincide
//		double rise = B(1) - A(1);
//		double run = B(0) - A(0);
//		// normal is (-rise, run)
//		if (rise >= run) { // x-coord of normal is bigger, project segments to y-axis
//			return IntervalIntervalIntersection(A(1), B(1), C(1), D(1));
//		}
//		else { // y-coord of normal is bigger, project segments to x-axis
//			return IntervalIntervalIntersection(A(0), B(0), C(0), D(0));
//		}
//	}
//}
//
//int SegmentSegmentIntersection3D(
//	const Eigen::RowVector3d& A,
//	const Eigen::RowVector3d& B,
//	const Eigen::RowVector3d& C,
//	const Eigen::RowVector3d& D,
//	NgonPlane facePlane)
//{
//	// project to plane
//	Eigen::RowVector3d N(facePlane.N);
//
//	// converting to regular vector to use max_element
//	std::vector<double> Nreg;
//	Nreg.push_back(std::abs(N(0)));
//	Nreg.push_back(std::abs(N(1)));
//	Nreg.push_back(std::abs(N(2)));
//
//	// find greatest coordinate from plane normal
//	// projection plane will be the plane generate by the OTHER coordinate axes
//	// e.g. if N.y is largest, project to xz-plane
//	int maxCoord = max_element(Nreg.begin(), Nreg.end()) - Nreg.begin();
//
//	std::vector<Eigen::RowVector3d> startVerts;
//	startVerts.push_back(A);
//	startVerts.push_back(B);
//	startVerts.push_back(C);
//	startVerts.push_back(D);
//
//	// project coords to the appropriate plane
//	// NOTE SYNTAX!!!!!!!!!!! 
//	std::vector<Eigen::RowVector2d, Eigen::aligned_allocator<Eigen::RowVector2d>> flatVerts;
//	for (int i = 0; i < startVerts.size(); ++i)
//	{
//		int counter = 0;
//		Eigen::RowVector2d tmpCoords;
//		for (int j = 0; j < 3; ++j)
//		{
//			if (j != maxCoord)
//			{
//				//std::cout << "counter=" << counter << " j=" << j << std::endl;
//				//std::cout << "startVerts[" << i << "](" << j << ")=" << startVerts[i](j) << std::endl;
//				tmpCoords(counter) = startVerts[i](j);
//				++counter;
//			}
//		}
//		//std::cout << "tmpCoords=" << tmpCoords << std::endl;
//		flatVerts.push_back(tmpCoords);
//	}
//
//	int ret = SegmentSegmentIntersection2D(flatVerts[0], flatVerts[1], flatVerts[2], flatVerts[3]);
//	return ret;
//
//}
//
//
//
//// Computes the list of edges to check in IsFacePolySelfIntersecting, N = # of edges
//// Note for edge 0, we only check it with edges 2, 3, 4, ..., N-2
//// And we only check pairs of the form (A, B) where B>A (removes duplicate checks)
//std::vector<std::vector<int>> GetEdgesToIntersect(int N)
//{
//	std::vector<std::vector<int>> edgesToCheck;
//
//	std::vector<int> edgePair;
//	for (int i = 0; i < N - 1; ++i)
//	{
//		std::vector<int> firstCoord;
//		firstCoord.push_back(i);
//		for (int j = i + 2; j < N - 1 + i; ++j)
//		{
//			if (j < N)
//			{
//				edgePair = firstCoord;
//				edgePair.push_back(j);
//				edgesToCheck.push_back(edgePair);
//
//
//			}
//		}
//
//	}
//
//	return edgesToCheck;
//}