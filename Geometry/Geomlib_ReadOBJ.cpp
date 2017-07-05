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

#include "Geomlib_ReadOBJ.h"

#include <cstring>

#include <Eigen/Core>

#include <igl/readOBJ.h>
#include <igl/list_to_matrix.h>

#include "ConversionUtilities.h"
#include "TriMesh.h"

using namespace Urho3D;

bool Geomlib::ReadOBJ(
	const Urho3D::String& obj_filename,
	Urho3D::Variant& tri_mesh,
	bool yup
)
{
	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	bool read_success = igl::readOBJ(obj_filename.CString(), V, F);
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

bool Geomlib::ReadOBJ(
	Urho3D::File* source,
	Urho3D::Variant& tri_mesh,
	bool yup
)
{
	std::vector<std::vector<float> > V;
	std::vector<std::vector<float> > TC;
	std::vector<std::vector<float> > N;
	std::vector<std::vector<int> > F;
	std::vector<std::vector<int> > FTC;
	std::vector<std::vector<int> > FN;

	// variables and constants to assist parsing the .obj file
	// Constant strings to compare against
	std::string v("v");
	std::string vn("vn");
	std::string vt("vt");
	std::string f("f");
	std::string tic_tac_toe("#");
#ifndef IGL_LINE_MAX
#define IGL_LINE_MAX 2048
#endif

	char line[IGL_LINE_MAX];
	int line_no = 1;

	while (!source->IsEof()) {
		//String line = source->ReadLine();
		char line[IGL_LINE_MAX];
		String lineString = source->ReadLine();
		//std::cout << "lineString = " << lineString.CString() << std::endl;
		//strncpy(line, source->ReadLine().CString(), IGL_LINE_MAX - 1);
		strncpy(line, lineString.CString(), IGL_LINE_MAX - 1);

		/////////////////////////////////////////////
		////////////////////////////////////////////

		char type[IGL_LINE_MAX];
		// Read first word containing type
		if (sscanf(line, "%s", type) == 1)
		{
			// Get pointer to rest of line right after type
			char * l = &line[strlen(type)];
			if (type == v)
			{
				double x[4];
				int count =
					sscanf(l, "%lf %lf %lf %lf\n", &x[0], &x[1], &x[2], &x[3]);
				if (count != 3 && count != 4)
				{
					fprintf(stderr,
						"Error: readOBJ() vertex on line %d should have 3 or 4 coordinates",
						line_no);
					//fclose(obj_file);
					return false;
				}
				std::vector<float > vertex(count);
				for (int i = 0; i<count; i++)
				{
					vertex[i] = x[i];
				}
				V.push_back(vertex);
			}
			else if (type == vn)
			{
				double x[3];
				int count =
					sscanf(l, "%lf %lf %lf\n", &x[0], &x[1], &x[2]);
				if (count != 3)
				{
					fprintf(stderr,
						"Error: readOBJ() normal on line %d should have 3 coordinates",
						line_no);
					//fclose(obj_file);
					return false;
				}
				std::vector<float > normal(count);
				for (int i = 0; i<count; i++)
				{
					normal[i] = x[i];
				}
				N.push_back(normal);
			}
			else if (type == vt)
			{
				double x[3];
				int count =
					sscanf(l, "%lf %lf %lf\n", &x[0], &x[1], &x[2]);
				if (count != 2 && count != 3)
				{
					fprintf(stderr,
						"Error: readOBJ() texture coords on line %d should have 2 "
						"or 3 coordinates (%d)",
						line_no, count);
					//fclose(obj_file);
					return false;
				}
				std::vector<float > tex(count);
				for (int i = 0; i<count; i++)
				{
					tex[i] = x[i];
				}
				TC.push_back(tex);
			}
			else if (type == f)
			{
				const auto & shift = [&V](const int i)->int
				{
					return i<0 ? i + V.size() : i - 1;
				};
				const auto & shift_t = [&TC](const int i)->int
				{
					return i<0 ? i + TC.size() : i - 1;
				};
				const auto & shift_n = [&N](const int i)->int
				{
					return i<0 ? i + N.size() : i - 1;
				};
				std::vector<int > f;
				std::vector<int > ftc;
				std::vector<int > fn;
				// Read each "word" after type
				char word[IGL_LINE_MAX];
				int offset;
				while (sscanf(l, "%s%n", word, &offset) == 1)
				{
					// adjust offset
					l += offset;
					// Process word
					long int i, it, in;
					if (sscanf(word, "%ld/%ld/%ld", &i, &it, &in) == 3)
					{
						f.push_back(shift(i));
						ftc.push_back(shift_t(it));
						fn.push_back(shift_n(in));
					}
					else if (sscanf(word, "%ld/%ld", &i, &it) == 2)
					{
						f.push_back(shift(i));
						ftc.push_back(shift_t(it));
					}
					else if (sscanf(word, "%ld//%ld", &i, &in) == 2)
					{
						f.push_back(shift(i));
						fn.push_back(shift_n(in));
					}
					else if (sscanf(word, "%ld", &i) == 1)
					{
						f.push_back(shift(i));
					}
					else
					{
						fprintf(stderr,
							"Error: readOBJ() face on line %d has invalid element format\n",
							line_no);
						//fclose(obj_file);
						return false;
					}
				}
				if (
					(f.size()>0 && fn.size() == 0 && ftc.size() == 0) ||
					(f.size()>0 && fn.size() == f.size() && ftc.size() == 0) ||
					(f.size()>0 && fn.size() == 0 && ftc.size() == f.size()) ||
					(f.size()>0 && fn.size() == f.size() && ftc.size() == f.size()))
				{
					// No matter what add each type to lists so that lists are the
					// correct lengths
					F.push_back(f);
					FTC.push_back(ftc);
					FN.push_back(fn);
				}
				else
				{
					fprintf(stderr,
						"Error: readOBJ() face on line %d has invalid format\n", line_no);
					//fclose(obj_file);
					return false;
				}
			}
			else if (strlen(type) >= 1 && (type[0] == '#' ||
				type[0] == 'g' ||
				type[0] == 's' ||
				strcmp("usemtl", type) == 0 ||
				strcmp("mtllib", type) == 0))
			{
				//ignore comments or other shit
			}
			else
			{
				//ignore any other lines
				fprintf(stderr,
					"Warning: readOBJ() ignored non-comment line %d:\n  %s",
					line_no,
					line);
			}
		}
		else
		{
			// ignore empty line
		}
		line_no++;

		//////////////////////////////////////////////////
		//////////////////////////////////////////////////
	}

	assert(F.size() == FN.size());
	assert(F.size() == FTC.size());

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