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

#include "Geomlib_PolylineIntersection.h"
#include "Geomlib_BestFitPlane.h"
#include <vector>
#include <algorithm>
#include <Eigen/Core>
#include "Polyline.h"

using Urho3D::Vector;
using Urho3D::Vector3;
using Urho3D::Vector2;

using namespace Urho3D;

// [t1, t2] interval of real number line; no promise that t1 <= t2
// [s1, s2] interval of real number line; no promise that s1 <= s2
// Returns:
//   1 if [t1, t2] and [s1, s2] have non-empty intersection
//   0 otherwise
int Geomlib::IntervalIntervalIntersection(
	double t1,
	double t2,
	double s1,
	double s2)
{
	if (t1 > t2) {
		double x = t1;
		t1 = t2;
		t2 = x;
	}
	if (s1 > s2) {
		double y = s1;
		s1 = s2;
		s2 = y;
	}

	if (s1 <= t1 && t1 <= s2) {
		return 1;
	}
	else if (s1 <= t2 && t2 <= s2) {
		return 1;
	}
	else if (t1 <= s1 && s1 <= t2) {
		return 1;
	}

	return 0;
}

//   1 if (A,B) and (C,D) have non-empty intersection
//   0 otherwise
int Geomlib::SegmentSegmentIntersection2D(
	Vector2& A,
	Vector2& B,
	Vector2& C,
	Vector2& D)
{
	// left & right half plane checks
	if (
		(A.x_ > 0 && B.x_ > 0) && // A,B in right half plane
		(C.x_ < 0 && D.x_ < 0)    // C,D in left half plane
		) {
		return 0;
	}
	if (
		(A.x_ < 0 && B.y_ < 0) && // A,B in left half plane
		(C.x_ > 0 && D.y_ > 0)    // C,D in right half plane
		) {
		return 0;
	}
	// upper & lower half plane checks
	if (
		(A.y_ > 0 && B.y_ > 0) && // A,B in upper half plane
		(C.y_ < 0 && D.y_ < 0)    // C,D in lower half plane
		) {
		return 0;
	}
	if (
		(A.y_ < 0 && B.y_ < 0) && // A,B in lower half plane
		(C.y_ > 0 && D.y_ > 0)    // C,D in upper half plane
		) {
		return 0;
	}

	// do C and D lie strictly on the same side of line through AB?
	double cval = (B.y_ - A.y_) * (C.x_ - A.x_) + (A.x_ - B.x_) * (C.y_ - A.y_);
	double dval = (B.y_ - A.y_) * (D.x_ - A.x_) + (A.x_ - B.x_) * (D.y_ - A.y_);
	if ((cval > 0 && dval > 0) || (cval < 0 && dval < 0)) {
		return 0;
	}
	// do A and B lie strictly on the same side of line through CD?
	double aval = (D.y_ - C.y_) * (A.x_ - C.x_) + (C.x_ - D.x_) * (A.y_ - C.y_);
	double bval = (D.y_ - C.y_) * (B.x_ - C.x_) + (C.x_ - D.x_) * (B.y_ - C.y_);
	if ((aval > 0 && bval > 0) || (aval < 0 && bval < 0)) {
		return 0;
	}

	// lines through AB and CD definitely intersect
	double k11 = B.x_ - A.x_;
	double k12 = C.x_ - D.x_;
	double k21 = B.y_ - A.y_;
	double k22 = C.y_ - D.y_;
	double z1 = C.x_ - A.x_;
	double z2 = C.y_ - A.y_;
	double prod1 = k11 * k22;
	double prod2 = k12 * k21;
	double prod3 = k22 * z1;
	double prod4 = k12 * z2;
    if (std::abs(prod1) > 1e16 || std::abs(prod2) > 1e16 || std::abs(prod3) > 1e16 || std::abs(prod4) > 1e16) {
		double shrink = 1e-6;
		Vector2 newA = shrink * A;
		Vector2 newB = shrink * B;
		Vector2 newC = shrink * C;
		Vector2 newD = shrink * D;
		return Geomlib::SegmentSegmentIntersection2D(newA, newB, newC, newD);
	}
	double denom = prod1 - prod2;
	double numer = prod3 - prod4;
	if (prod1 == prod2) {
		denom = 0;
	}

	const double eps = 1e-8;

	if (denom > eps || -denom > eps) {
		// there is unique point of intersection: does it lie in segment AB?
		double t = (k22 * z1 - k12 * z2) / denom;
		return 0 <= t && t <= 1;
	}
	else {
		// lines through AB and CD coincide
		double rise = B.y_ - A.y_;
		double run = B.x_ - A.x_;
		// normal is (-rise, run)
		if (rise >= run) { // x-coord of normal is bigger, project segments to y-axis
			return Geomlib::IntervalIntervalIntersection(A.y_, B.y_, C.y_, D.y_);
		}
		else { // y-coord of normal is bigger, project segments to x-axis
			return Geomlib::IntervalIntervalIntersection(A.x_, B.x_, C.x_, D.x_);
		}
	}
}

	int Geomlib::SegmentSegmentIntersection3D(
		Vector3& A,
		Vector3& B,
		Vector3& C,
		Vector3& D,
		Vector3& normal)
	{
		

		// converting to regular vector to use max_element
		std::vector<double> Nreg;
		Nreg.push_back(std::abs(normal.x_));
		Nreg.push_back(std::abs(normal.y_));
		Nreg.push_back(std::abs(normal.z_));

		// find greatest coordinate from plane normal
		// projection plane will be the plane generate by the OTHER coordinate axes
		// e.g. if N.y is largest, project to xz-plane
		int maxCoord = std::max_element(Nreg.begin(), Nreg.end()) - Nreg.begin();


		Vector<Vector3> startVerts;
		startVerts.Push(A);
		startVerts.Push(B);
		startVerts.Push(C);
		startVerts.Push(D);

		// project coords to the appropriate plane
		// NOTE SYNTAX!!!!!!!!!!! 
		std::vector<Eigen::RowVector2d, Eigen::aligned_allocator<Eigen::RowVector2d>> flatVerts;
		//Vector<Vector2> flatVerts;
		for (int i = 0; i < startVerts.Size(); ++i)
		{
			int counter = 0;
			Eigen::RowVector2d tmpCoords;
			//Vector2 tmpCoords;
			for (int j = 0; j < 3; ++j)
			{
				if (j != maxCoord)
				{
					//std::cout << "counter=" << counter << " j=" << j << std::endl;
					//std::cout << "startVerts[" << i << "](" << j << ")=" << startVerts[i](j) << std::endl;
					if (j == 0)
						tmpCoords(counter) = startVerts[i].x_;
					else if (j == 1)
						tmpCoords(counter) = startVerts[i].y_;
					else if (j == 2)
						tmpCoords(counter) = startVerts[i].z_;
					++counter;
				}
			}
			//std::cout << "tmpCoords=" << tmpCoords << std::endl;
			flatVerts.push_back(tmpCoords);
		}

		Vector2 newA(flatVerts[0](0), flatVerts[0](1));
		Vector2 newB(flatVerts[1](0), flatVerts[1](1));
		Vector2 newC(flatVerts[2](0), flatVerts[2](1));
		Vector2 newD(flatVerts[3](0), flatVerts[3](1));

		int ret = Geomlib::SegmentSegmentIntersection2D(
			newA,
			newB,
			newC,
			newD);

		return ret;

	}

	bool Geomlib::HasSelfIntersection(const Urho3D::Variant polyline)
	{
		VariantVector verts = Polyline_ComputeSequentialVertexList(polyline);

		// This will only work for planar polylines!
		Vector3 point, normal;
		Geomlib::BestFitPlane(Polyline_ComputePointCloud(polyline), point, normal);

		bool closed = Polyline_IsClosed(polyline);
		int counter = verts.Size()-4;
		if (counter < 4)
			return false;
		
		for (int i = 0; i < counter; ++i) {
			Vector3 A = verts[i].GetVector3();
			Vector3 B = verts[i + 1].GetVector3();
			for (int j = i + 2; j < counter+2; ++j) {
				Vector3 C = verts[j].GetVector3();
				Vector3 D = verts[j + 1].GetVector3();
				int ret = Geomlib::SegmentSegmentIntersection3D(A, B, C, D, normal);
				if (ret == 1)
					return true;
			}
		}
		return false;
	}

	int Geomlib::GetSelfIntersections(const Urho3D::Variant polyline, Urho3D::Vector<Urho3D::Vector3>& intersections, Urho3D::Variant& revised_polyline)
	{
		VariantVector verts = Polyline_ComputeSequentialVertexList(polyline);
		Vector<Vector3> revised_verts;
		for (int i = 0; i < verts.Size(); ++i)
			revised_verts.Push(verts[i].GetVector3());

		// This will only work for planar polylines!
		Vector3 point, normal;
		Geomlib::BestFitPlane(Polyline_ComputePointCloud(polyline), point, normal);

		bool closed = Polyline_IsClosed(polyline);
		int counter = verts.Size() - 4;
		if (counter < 4)
			return 0;

		for (int i = 0; i < counter; ++i) {
			Vector3 A = verts[i].GetVector3();
			Vector3 B = verts[i + 1].GetVector3();
			for (int j = i + 2; j < counter + 2; ++j) {
				Vector3 C = verts[j].GetVector3();
				Vector3 D = verts[j + 1].GetVector3();
				int ret = Geomlib::SegmentSegmentIntersection3D(A, B, C, D, normal);
				if (ret == 1) {
					Vector3 point;
					int success = GetPointOfIntersection(A, B, C, D, point);
					if (success) {
						intersections.Push(point);
						InsertSelfIntersectionsAsPoint(A, B, C, D, point, revised_verts);
					}
				}
			}
		}
		revised_polyline = Geomlib::Polyline_Make_With_Intersections(revised_verts);
		return 1;
	}

	// inserts the point into the vertex list between A, B and C, D 
	int Geomlib::InsertSelfIntersectionsAsPoint(
		const Urho3D::Vector3& A,
		const Urho3D::Vector3& B,
		const Urho3D::Vector3& C,
		const Urho3D::Vector3& D,
		const Urho3D::Vector3& point,
		Urho3D::Vector<Urho3D::Vector3>& verts)
	{
		// Assuming this inserts before the destination element?
		Vector<Vector3>::Iterator it_D= verts.Find(D);
		verts.Insert(it_D, point);

		Vector<Vector3>::Iterator it_B = verts.Find(B);
		verts.Insert(it_B, point);

		return 0;
	}

	int Geomlib::GetPointOfIntersection( const Urho3D::Vector3 & A, const Urho3D::Vector3 & B, const Urho3D::Vector3 & C, const Urho3D::Vector3 & D, Urho3D::Vector3& point)
	{
		double ta, tb;
		bool success = Geomlib::ShortestDistance(A, B, C, D, ta, tb);
		if (ta < 0 || ta > 1 || tb < 0 || tb >1)
			return 0;

		// find the points on the line segments
		Vector3 pa = A + ta*(B - A);
		Vector3 pb = C + tb*(D - C);

		// if the distance between the points is small, this is a true intersection
		double dist = (pb - pa).Length();
		if (dist < 0.0001) {
			point = pa;
			return 1;
		}
		
		return 0;
	}

	/// returns parameters on each line. If they are between 0-1, the closest point lies on the line. Otherwise,
	/// the point is on the ray defined by the line direction.
	/// if return is false, the closest point could not be calculated (probably due to the lines being parallel)
	bool Geomlib::ShortestDistance(
		const Vector3& A,
		const Vector3& B,
		const Vector3& C,
		const Vector3& D, 
		double& ta, double& tb)
	{

		Vector3 u = B - A;
		Vector3 v = D - C;
		Vector3 w = A - C;

		double dot = u.Normalized().DotProduct(v.Normalized());

        if (std::abs(dot - 1.0) < 10e-7)
			return false;

		double uu = u.DotProduct(u);
		double uv = u.DotProduct(v);
		double vv = v.DotProduct(v);
		double uw = u.DotProduct(w);
		double vw = v.DotProduct(w);

		double t = 1.0 / (uu * vv - uv * uv);
		ta = (uv * vw - vv * uw) * t;
		tb = (uu * vw - uv * uw) * t;

		return true;

	}

	Urho3D::Variant Geomlib::Polyline_Make_With_Intersections(const Urho3D::Vector<Urho3D::Vector3>& vertexList)
	{
		Variant earlyRet;
		if (vertexList.Size() < 2) {
			return earlyRet;
		}

		Vector<Variant> vertices;
		Vector<Variant> edges;

		for (int i = 0; i < vertexList.Size(); ++i) {
			vertices.Push(Variant(vertexList[i]));
			edges.Push(Variant(i));
		}
		VariantMap var_map;
		var_map["type"] = Variant(String("Polyline"));
		var_map["vertices"] = Variant(vertices);
		var_map["edges"] = Variant(edges);

		return Variant(var_map);
	}
