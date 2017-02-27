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


#include "Geomlib_PolylineLoft.h"

#include <iostream>

#include "Geomlib_PolylineDivide.h"
#include "Polyline.h"
#include "TriMesh.h"

bool Geomlib::PolylineLoft(
	const Urho3D::Vector<Urho3D::Variant>& polylines,
	Urho3D::Variant& mesh
)
{
	using Urho3D::Variant;
	using Urho3D::Vector3;
	using Urho3D::Vector;

	// Verify inputs
	if (polylines.Size() <= 1) {
		std::cerr << "ERROR: Geomlib::PolylineLoft --- polylines.Size() <= 1\n";
		mesh = Variant();
		return false;
	}
	assert(polylines.Size() > 1);
	for (unsigned i = 0; i < polylines.Size(); ++i) {
		if (!Polyline_Verify(polylines[i])) {
			std::cerr << "ERROR: Geomlib::PolylineLoft --- invalid polyline in polyline list\n";
			mesh = Variant();
			return false;
		}
	}

	// Prepare polylines for lofting by dividing them all into the same number of parts
	/*
	Vector<Variant> divPlines;
	for (unsigned i = 0; i < polylines.Size(); ++i) {
		Variant pline = polylines[i];
		Variant plineDiv;
		bool success = Geomlib::PolylineDivide(pline, n, plineDiv);
		if (!success) {
			std::cerr << "ERROR: Geomlib::PolylineLoft -- division failed on one of the input polylines\n";
			mesh = Variant();
			return false;
		}
		divPlines.Push(plineDiv);
	}
	*/
	int n = -1;
	// Ensure all polylines have the same number of vertices in sequential vertex list
	{
		Urho3D::VariantVector verts = Polyline_ComputeSequentialVertexList(polylines[0]);
		int seqNum = Polyline_GetSequentialVertexNumber(polylines[0]);
		if (seqNum < 0) {
			std::cerr << "ERROR: Geomlib::PolylineLoft -- seqNum < 0\n";
			return false;
		}
		for (unsigned i = 1; i < polylines.Size(); ++i) {
			if (seqNum != Polyline_GetSequentialVertexNumber(polylines[i])) {
				std::cerr << "ERROR: Geomlib::PolylineLoft -- all polylines must have the same number of vertices\n";
				return false;
			}
		}
		n = seqNum;
	}
	if (n < 0) {
		std::cerr << "ERROR: Geomlib::PolylineLoft -- n < 0\n";
		return false;
	}

	// Construct vertex list for output tri mesh

	Urho3D::VariantVector totalVertexList;

	/*
	for (unsigned i = 0; i < divPlines.Size(); ++i) {
		Variant pline = divPlines[i];
		Urho3D::VariantVector verts = Polyline_ComputeSequentialVertexList(pline);
		if (verts.Size() != n + 1) {
			std::cerr << "ERROR: Geomlib::PolylineLoft --- verts.Size() != n + 1\n";
			mesh = Variant();
			return false;
		}
		else {
			for (unsigned i = 0; i < verts.Size(); ++i) {
				totalVertexList.Push(verts[i].GetVector3());
			}
		}
	}
	*/

	for (unsigned i = 0; i < polylines.Size(); ++i) {
		Variant pline = polylines[i];
		Urho3D::VariantVector verts = Polyline_ComputeSequentialVertexList(pline);
		if (verts.Size() != n) {
			std::cerr << "ERROR: Geomlib::PolylineLoft --- verts.Size() != n\n";
			mesh = Variant();
			return false;
		}
		else {
			for (unsigned i = 0; i < verts.Size(); ++i) {
				totalVertexList.Push(verts[i].GetVector3());
			}
		}
	}

	// totalVertexList has:
	// n + 1 verts for polyline 1
	// n + 1 verts for polyline 2
	// ...
	// n + 1 verts for polyline k
	// (n >= 1, k >= 2)

	// Construct face list for output tri mesh

	int numSeams = (int)polylines.Size();

	Urho3D::VariantVector faceList;
	for (int i = 0; i < numSeams - 1; ++i) {
		for (int j = 0; j < n - 1; ++j) {
			int v0 = (i * n) + j;
			int v1 = v0 + n;
			int v2 = v0 + 1;
			int v3 = v1 + 1;

			faceList.Push(v0);
			faceList.Push(v1);
			faceList.Push(v2);
			faceList.Push(v2);
			faceList.Push(v1);
			faceList.Push(v3);
		}
	}

	// Construct the output tri mesh

	Variant vertices(totalVertexList);
	Variant faces(faceList);

	mesh = TriMesh_Make(vertices, faces);

	return true;
}