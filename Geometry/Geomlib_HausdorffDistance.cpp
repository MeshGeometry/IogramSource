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


#include "Geomlib_HausdorffDistance.h"

#pragma warning(push, 0)
#include <igl/hausdorff.h>
#pragma warning(pop)

#include "ConversionUtilities.h"
#include "TriMesh.h"

bool Geomlib::TriMesh_HausdorffDistance(
	const Urho3D::Variant& mesh1,
	const Urho3D::Variant& mesh2,
	float& distance
)
{
	if (!TriMesh_Verify(mesh1) || !TriMesh_Verify(mesh2)) {
		return false;
	}

	Eigen::MatrixXf V1, V2;
	Eigen::MatrixXi F1, F2;
	IglMeshToMatrices(mesh1, V1, F1);
	IglMeshToMatrices(mesh2, V2, F2);

	float d = -1.0f;
	igl::hausdorff(V1, F1, V2, F2, d);
	if (d >= 0.0f) {
		distance = d;
		return true;
	}
	else {
		return false;
	}
}