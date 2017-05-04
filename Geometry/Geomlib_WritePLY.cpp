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

#include "Geomlib_WritePLY.h"

#include <Eigen/Core>

#include <igl/writePLY.h>

#include "ConversionUtilities.h"
#include "TriMesh.h"

using namespace Urho3D;

bool Geomlib::WritePLY(
	const Urho3D::String& ply_filename,
	const Urho3D::Variant& tri_mesh,
	bool zup
)
{
	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	bool val = IglMeshToMatrices(tri_mesh, V, F);
	if (!val) {
		return false;
	}

	if (zup) {
		Eigen::MatrixXf W = V;
		W.col(1) = -1 * V.col(2);
		W.col(2) = V.col(1);
		V = W;
	}

	bool write_success = igl::writePLY(ply_filename.CString(), V, F);
	return write_success;
}