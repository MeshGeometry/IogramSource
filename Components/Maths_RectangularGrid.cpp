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


#include "Maths_RectangularGrid.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

#include "Polyline.h"

using namespace Urho3D;

String Maths_RectangularGrid::iconTexture = "Textures/Icons/Maths_RectangularGrid.png";

Maths_RectangularGrid::Maths_RectangularGrid(Urho3D::Context* context) : IoComponentBase(context, 5, 2)
{
	SetName("RectangularGrid");
	SetFullName("Rectangular Grid");
	SetDescription("Generates a planar rectangular grid from transform, number of cells and cell size");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("transform");
	inputSlots_[0]->SetVariableName("T");
	inputSlots_[0]->SetDescription("Transform: sets plane for rectangular grid");
	inputSlots_[0]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[0]->SetDefaultValue(Matrix3x4::IDENTITY);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("numberX");
	inputSlots_[1]->SetVariableName("NX");
	inputSlots_[1]->SetDescription("Number of cells in X direction");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDefaultValue(10);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("numberZ");
	inputSlots_[2]->SetVariableName("NZ");
	inputSlots_[2]->SetDescription("Number of cells in Z direction");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDefaultValue(10);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("sizeX");
	inputSlots_[3]->SetVariableName("x");
	inputSlots_[3]->SetDescription("Size of cells in X direction");
	inputSlots_[3]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[3]->SetDefaultValue(1.0f);
	inputSlots_[3]->DefaultSet();

	inputSlots_[4]->SetName("sizeZ");
	inputSlots_[4]->SetVariableName("z");
	inputSlots_[4]->SetDescription("size of cells in Z direction");
	inputSlots_[4]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[4]->SetDefaultValue(1.0f);
	inputSlots_[4]->DefaultSet();

	outputSlots_[0]->SetName("Rectangular Grid Vertices");
	outputSlots_[0]->SetVariableName("G");
	outputSlots_[0]->SetDescription("Vertices");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[1]->SetName("Rectangular Grid Polylines");
	outputSlots_[1]->SetVariableName("P");
	outputSlots_[1]->SetDescription("Cell Polylines");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[1]->SetDataAccess(DataAccess::LIST);
}

void Maths_RectangularGrid::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	//check that we have the right kind of data
	bool res = false;
	if (inSolveInstance[0].GetType() == VAR_MATRIX3X4 &&
		(inSolveInstance[1].GetType() == VAR_INT || inSolveInstance[1].GetType() == VAR_FLOAT) &&
		(inSolveInstance[2].GetType() == VAR_INT || inSolveInstance[2].GetType() == VAR_FLOAT) &&
		(inSolveInstance[3].GetType() == VAR_INT || inSolveInstance[3].GetType() == VAR_FLOAT) &&
		(inSolveInstance[4].GetType() == VAR_INT || inSolveInstance[4].GetType() == VAR_FLOAT) )
	{
		res = true;
	}

	if (res)
	{

		// keep track of two lists, one for rows of vertices, the other for closed polyline cells. 
		Matrix3x4 trans = inSolveInstance[0].GetMatrix3x4();
		int x_numb = inSolveInstance[1].GetInt();
		int z_numb = inSolveInstance[2].GetInt();
		float x_size = inSolveInstance[3].GetFloat();
		float z_size = inSolveInstance[4].GetFloat();

		// Set up x- and z grid "basis" vectors
		Vector3 origin = Vector3::ZERO;
		Vector3 x_basis = Vector3(x_size, 0.0f, 0.0f);
		Vector3 z_basis = Vector3(0.0f, 0.0f, z_size);

		VariantVector gridList;
		VariantVector centreList;

		// Set up base list in x-direction
		VariantVector base_list;
		for (int i = 0; i < x_numb + 1; ++i)
		{
			base_list.Push(Variant(origin + i*x_basis));
		}

		// Add rows in z-direction
		for (int i = 0; i < z_numb + 1; ++i)
		{
			VariantVector currList;
			for (int j = 0; j < base_list.Size(); ++j)
			{
				currList.Push(Variant(base_list[j].GetVector3() + i*z_basis));
			}
			gridList.Push(currList);
		}

		// Now create the polylines
		VariantVector polylineList;
		for (int i = 0; i < z_numb; ++i)
		{
			for (int j = 0; j < x_numb; ++j)
			{
				VariantMap currRect; //polyline
				VariantVector verts;

				// seems kind of hacky -- the key is that the vertices are stored in a big list. 
				// rows begin at i*(x_numb + 1), i = 0...(y_numb+1)
				verts.Push(gridList[i*(x_numb + 1) + j].GetVector3());
				verts.Push(gridList[i*(x_numb + 1) + (j + 1)].GetVector3());
				verts.Push(gridList[(i + 1)*(x_numb + 1) + (j + 1)].GetVector3());
				verts.Push(gridList[(i + 1)*(x_numb + 1) + j].GetVector3());
				verts.Push(gridList[i*(x_numb + 1) + j].GetVector3());

				currRect["vertices"] = verts;
				polylineList.Push(currRect);
			}
		}

		// Create the Square Centres
		Vector3 centre_origin = origin + x_basis / 2 + z_basis / 2;
		for (int i = 0; i < x_numb; ++i)
		{
			for (int j = 0; j < z_numb; ++j)
			{
				centreList.Push(Variant(centre_origin + i*x_basis + j*z_basis));
			}
		}

		// Now transform everything. 
		VariantVector gridList_transformed;
		VariantVector centreList_transformed;
		VariantVector polylineList_transformed;

		// transform the coordinates of the cell corners (not currently being output)
		for (int i = 0; i < gridList.Size(); ++i)
		{
			Vector3 curCoord = gridList[i].GetVector3();
			Vector3 newCoord = trans*curCoord;
			gridList_transformed.Push(newCoord);
		}

		// transform the coordinates of the centres
		for (int i = 0; i <centreList.Size(); ++i)
		{
			Vector3 curCoord = centreList[i].GetVector3();
			Vector3 newCoord = trans*curCoord;
			Variant sanityVar(newCoord);
			sanityVar;
			//centreList_transformed.Push(Variant(newCoord));
			centreList_transformed.Push(sanityVar);
		}

		// transform the polylines
		for (int i = 0; i < polylineList.Size(); ++i)
		{
			VariantMap curPoly = polylineList[i].GetVariantMap();
			VariantVector newVertexList;
			VariantVector vertexList = curPoly["vertices"].GetVariantVector();

			// apply transform to vertex list
			for (unsigned i = 0; i < vertexList.Size(); ++i) {
				Vector3 vert = vertexList[i].GetVector3();
				Vector3 transformedVert = trans*vert;
				newVertexList.Push(Variant(transformedVert));
			}

			// newVertexList is ready
			Variant newVertices(newVertexList);
			curPoly["vertices"] = newVertices;
			Variant geomOut = curPoly;

			//polylineList_transformed.Push(geomOut);
			polylineList_transformed.Push(Polyline_Make(newVertexList));
		}

		outSolveInstance[0] = centreList_transformed;
		outSolveInstance[1] = polylineList_transformed;
	}
}
