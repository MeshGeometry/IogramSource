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


#include "Geomlib_SmoothPolyline.h"

#include <iostream>
#include <vector>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Vector3.h>

#include "Polyline.h"

using Urho3D::Variant;
using Urho3D::VariantVector;
using Urho3D::Vector3;

bool Geomlib::SmoothPolyline(
	const Urho3D::Variant& polylineIn,
	Urho3D::Variant& polylineOut
)
{
	if (!Polyline_Verify(polylineIn)) {
		polylineOut = Variant();
		return false;
	}

	const VariantVector vertexList = Polyline_ComputeSequentialVertexList(polylineIn);

	VariantVector newVertexList;

	if (vertexList.Size() == 0) {
		polylineOut = Variant();
		return false;
	}

	if (vertexList.Size() == 1) { // just one vertex; smooth polyline is same as original
		polylineOut = polylineIn;
		return true;
	}

	assert(vertexList.Size() >= 2);

	unsigned endIndex = vertexList.Size() - 1;

	/*
	Conceptual ALERT:
	  If vertexList[0].GetVector3() and vertexList[endIndex].GetVector3() store the same coordinates,
	  we view this as a closed polyline, and in particular we view those vertices as identical.
	  However, for interior vertices, we STILL view vertexList[i].GetVector3() and vertexList[i + 1].GetVector3()
	  as distinct, even when they store equal coordinates.
	*/
	bool isClosed = vertexList[0].GetVector3() == vertexList[endIndex].GetVector3();

	if (vertexList.Size() == 2 && isClosed) { // two identical vertices; smooth polyline is same as original
		polylineOut = polylineIn;
		return true;
	}

	Vector3 start, end;
	if (isClosed) {
		unsigned startPrevIndex = endIndex - 1;
		start =
			0.125f * vertexList[startPrevIndex].GetVector3() +
			0.75f * vertexList[0].GetVector3() +
			0.125f * vertexList[1].GetVector3();

		unsigned endNextIndex = 1;
		end =
			0.125f * vertexList[endIndex - 1].GetVector3() +
			0.75f * vertexList[endIndex].GetVector3() +
			0.125f * vertexList[endNextIndex].GetVector3();

		// Ultra confusing. Notice that start == end.
	}
	else {
		start = 0.875f * vertexList[0].GetVector3() + 0.125f * vertexList[1].GetVector3();
		end = 0.125f * vertexList[endIndex - 1].GetVector3() + 0.875f * vertexList[endIndex].GetVector3();
	}
	newVertexList.Push(start);

	// Compute new edge vertex for edge [i - 1, i] at each step.
	// If vertex i is an interior vertex, update its position.
	for (unsigned i = 1; i < vertexList.Size(); ++i) {
		Vector3 newEdgeVertex = 0.5f * vertexList[i - 1].GetVector3() + 0.5f * vertexList[i].GetVector3();
		newVertexList.Push(newEdgeVertex);
		if (i != endIndex) {
			Vector3 prev = vertexList[i - 1].GetVector3();
			Vector3 next = vertexList[i + 1].GetVector3();
			Vector3 iNewVertex =
				0.125f * prev +
				0.75f * vertexList[i].GetVector3() +
				0.125f * next;
			newVertexList.Push(iNewVertex);
		}
	}

	newVertexList.Push(end);

	polylineOut = Polyline_Make(newVertexList);
	return true;
}
