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

#include "Geomlib_ReadOFF.h"
#include <iostream>
#include <vector>

#include <Eigen/Core>

#include <igl/readOFF.h>
#include <igl/list_to_matrix.h>

#include "ConversionUtilities.h"
#include "TriMesh.h"

using namespace Urho3D;

bool Geomlib::ReadOFF(
	const Urho3D::String& off_filename,
	Urho3D::Variant& tri_mesh,
	bool yup
)
{
	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	bool read_success = igl::readOFF(off_filename.CString(), V, F);
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

bool Geomlib::ReadOFF(
	Urho3D::File* source,
	Urho3D::Variant& tri_mesh,
	bool yup
)
{
	using namespace std;

	std::vector<std::vector<float> > V;
	std::vector<std::vector<float> > N;
	std::vector<std::vector<int> > F;
	std::vector<std::vector<float> > C;

	// First line is always OFF
	char header[1000];
	const std::string OFF("OFF");
	const std::string NOFF("NOFF");
	const std::string COFF("COFF");

	if (source->IsEof()) return false;
	String headerString = source->ReadLine();
	strncpy(header, headerString.CString(), 999);

	if (!(
		string(header).compare(0, OFF.length(), OFF) == 0 ||
		string(header).compare(0, COFF.length(), COFF) == 0 ||
		string(header).compare(0, NOFF.length(), NOFF) == 0
		)) {
		printf("Error: Geomlib::ReadOFF() first line should be OFF or NOFF or COFF, not %s...", header);
		return false;
	}
	bool has_normals = string(header).compare(0, NOFF.length(), NOFF) == 0;
	bool has_vertexColors = string(header).compare(0, COFF.length(), COFF) == 0;
	// Second line is #vertices #faces #edges
	int number_of_vertices;
	int number_of_faces;
	int number_of_edges;
	char tic_tac_toe;
	char line[1000];
	bool still_comments = true;
	while (still_comments)
	{
		String lineString = source->ReadLine();
		strncpy(line, lineString.CString(), 999);
		//fgets(line, 1000, off_file);
		still_comments = (line[0] == '#' || line[0] == '\n');
	}
	sscanf(line, "%d %d %d", &number_of_vertices, &number_of_faces, &number_of_edges);
	V.resize(number_of_vertices);
	if (has_normals)
		N.resize(number_of_vertices);
	if (has_vertexColors)
		C.resize(number_of_vertices);
	F.resize(number_of_faces);
	//printf("%s %d %d %d\n",(has_normals ? "NOFF" : "OFF"),number_of_vertices,number_of_faces,number_of_edges);
	// Read vertices
	for (int i = 0; i<number_of_vertices;)
	{
		char line[1000];
		String lineString = source->ReadLine();
		strncpy(line, lineString.CString(), 999);
		double x, y, z, nx, ny, nz;
		if (sscanf(line, "%lg %lg %lg %lg %lg %lg", &x, &y, &z, &nx, &ny, &nz) >= 3)
		{
			std::vector<float > vertex;
			vertex.resize(3);
			vertex[0] = x;
			vertex[1] = y;
			vertex[2] = z;
			V[i] = vertex;

			if (has_normals)
			{
				std::vector<float > normal;
				normal.resize(3);
				normal[0] = nx;
				normal[1] = ny;
				normal[2] = nz;
				N[i] = normal;
			}

			if (has_vertexColors)
			{
				C[i].resize(3);
				C[i][0] = nx / 255.0;
				C[i][1] = ny / 255.0;
				C[i][2] = nz / 255.0;
			}
			i++;
		}
		else if (
			sscanf(line, "%[#]", &tic_tac_toe) >= 1)
		{
			// this line is a comment
		}
		else
		{
			printf("Error: bad line (%d)\n", i);
			if (source->IsEof())
			{
				return false;
			}
		}
	}
	// Read faces
	for (int i = 0; i<number_of_faces;)
	{
		char line[1000];
		String lineString = source->ReadLine();
		strncpy(line, lineString.CString(), 999);

		std::vector<int > face;
		int valence;
		if (sscanf(line, "%d", &valence) == 1)
		{
			face.resize(valence);

			if (valence == 3) {
				int val, i0, i1, i2;
				if (sscanf(line, "%d %d %d %d", &val, &i0, &i1, &i2) > 0) {
					face[0] = i0;
					face[1] = i1;
					face[2] = i2;
				}
			}
			else if (valence == 4) {
				int val, i0, i1, i2, i3;
				if (sscanf(line, "%d %d %d %d %d", &val, &i0, &i1, &i2, &i3) > 0) {
					face[0] = i0;
					face[1] = i1;
					face[2] = i2;
					face[3] = i3;
				}
			}

			F[i] = face;
			i++;
		}
		else if (
			sscanf(line, "%[#]", &tic_tac_toe) >= 1)
		{
			// this line is a comment
		}
		else
		{
			printf("Error: bad line\n");
			return false;
		}
	}

	const char* format = "Failed to cast %s to matrix: min (%d) != max (%d)\n";

	Eigen::MatrixXf xV; // xV = matrix V
	bool V_rect = igl::list_to_matrix(V, xV);
	if (!V_rect) {
		printf(format, "xV", igl::min_size(V), igl::max_size(V));
		return false;
	}

	Eigen::MatrixXi xF; // xF = matrix F
	bool F_rect = igl::list_to_matrix(F, xF);
	if (!F_rect) {
		printf(format, "xF", igl::min_size(F), igl::max_size(F));
		return false;
	}

	// xV, xF record the mesh at this stage

	if (yup) {
		Eigen::MatrixXf xW = xV;
		xW.col(1) = xV.col(2);
		xW.col(2) = -1 * xV.col(1);
		xV = xW;
	}

	tri_mesh = TriMesh_Make(xV, xF);
	if (!TriMesh_Verify(tri_mesh)) {
		return false;
	}


	return true;
}
