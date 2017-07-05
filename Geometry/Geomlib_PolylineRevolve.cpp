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


#include "Geomlib_PolylineRevolve.h"

#include <iostream>

#include "Geomlib_PolylineRefine.h"
#include "Geomlib_PolylineLoft.h"
#include "Geomlib_ConstructTransform.h"
#include "Geomlib_TransformVertexList.h"
#include "Geomlib_PolylineDivide.h"
#include "TriMesh.h"
#include "Polyline.h"



bool Geomlib::PolylineRevolve(
	const Urho3D::Vector3& axis,
	const Urho3D::Variant& section,
	const int resolution,
	Urho3D::Variant& mesh
)
{
	using Urho3D::Variant;
	using Urho3D::VariantType;
	using Urho3D::VariantMap;
	using Urho3D::VariantVector;
	using Urho3D::Vector;
	using Urho3D::Vector3;

	/////////////////////////
	// VERIFY INPUT POLYLINES


	if (!Polyline_Verify(section)) {
		mesh = Variant();
		return false;
	}

	//////////////////
	// CLEAN & EXTRACT

	Variant cleanRail = Polyline_Clean(axis);
	VariantVector cleanRailVerts = Polyline_ComputeSequentialVertexList(axis);

	Variant cleanSection = Polyline_Clean(section);
	VariantVector cleanSectionVerts = Polyline_ComputeSequentialVertexList(section);
	Vector3 sectionStart = cleanSectionVerts[0].GetVector3();

	Vector<Variant> sections;

	// compute angle of each section
	float theta = 360.0f / (float)(resolution-1);



	// iterate through vertices of the rail curve, create affine transformation of section polyline
	for (int i = 0; i < resolution; ++i) {

		Urho3D::Quaternion curr_rot = Urho3D::Quaternion(i*theta, axis);

		Urho3D::Matrix3x4 curr_trans = Geomlib::ConstructTransform(curr_rot);

		Vector<Variant> newVerts = Geomlib::TransformVertexList(curr_trans, cleanSectionVerts);
		Variant TranslatedSection = Polyline_Make(newVerts);
		sections.Push(TranslatedSection);
	}
	Geomlib::PolylineLoft(sections, mesh);

	return true;
}


