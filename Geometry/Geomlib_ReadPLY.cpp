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

#include "Geomlib_ReadPLY.h"

#include <iostream>
#include <vector>

#include <Eigen/Core>

#include <igl/readPLY.h>
#include <igl/list_to_matrix.h>

#include "ConversionUtilities.h"
#include "TriMesh.h"

using namespace Urho3D;

bool Geomlib::ReadPLY(
	const Urho3D::String& ply_filename,
	Urho3D::Variant& tri_mesh,
	bool yup
)
{
	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	bool read_success = igl::readPLY(ply_filename.CString(), V, F);
	if (!read_success) {
		return false;
	}

	if (yup) {
		Eigen::MatrixXf W = V;
		W.col(1) = V.col(2);
		W.col(2) = -1 * V.col(1);
		V = W;
	}

	tri_mesh = TriMesh_Make(V, F);
	if (!TriMesh_Verify(tri_mesh)) {
		return false;
	}
	return true;
}

bool Geomlib::ReadPLY(
	Urho3D::File* source,
	Urho3D::Variant& tri_mesh,
	bool yup
)
{
	std::vector<std::vector<float> > vV;
	std::vector<std::vector<int> > vF;
	std::vector<std::vector<float> > vN;
	std::vector<std::vector<float> > vUV;
	FILE* ply_file = (FILE*)source->GetHandle();
	bool read_success = igl::readPLY(ply_file, vV, vF, vN, vUV);
	if (!read_success) {
		return false;
	}

	Eigen::MatrixXf xV; // xV = matrix V
	bool V_rect = igl::list_to_matrix(vV, xV);
	if (!V_rect) {
		return false;
	}

	Eigen::MatrixXi xF; // xF = matrix F
	bool F_rect = igl::list_to_matrix(vF, xF);
	if (!F_rect) {
		return false;
	}

	if (yup) {
		Eigen::MatrixXf W = xV;
		W.col(1) = xV.col(2);
		W.col(2) = -1 * xV.col(1);
		xV = W;
	}

	tri_mesh = TriMesh_Make(xV, xF);
	if (!TriMesh_Verify(tri_mesh)) {
		return false;
	}
	std::cout << "return true from Geomlib_ReadPLY" << std::endl;
	return true;
}
