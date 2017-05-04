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


#include "Maths_HexGrid.h"
#include "Polyline.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_HexGrid::iconTexture = "Textures/Icons/Maths_HexGrid.png";

Maths_HexGrid::Maths_HexGrid(Urho3D::Context* context) : IoComponentBase(context, 5, 2)
{
	SetName("HexagonalGrid");
	SetFullName("Hexagonal Grid");
	SetDescription("Generates a planar hexagonal grid");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("transform");
	inputSlots_[0]->SetVariableName("T");
	inputSlots_[0]->SetDescription("Transform: sets plane for hex grid");
	inputSlots_[0]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[0]->SetDefaultValue(Matrix3x4::IDENTITY);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("numberX");
	inputSlots_[1]->SetVariableName("NX");
	inputSlots_[1]->SetDescription("Number of cells in X direction");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDefaultValue(4);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("numberY");
	inputSlots_[2]->SetVariableName("NY");
	inputSlots_[2]->SetDescription("Number of cells in Y direction");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDefaultValue(4);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("numberZ");
	inputSlots_[3]->SetVariableName("NZ");
	inputSlots_[3]->SetDescription("Number of cells in Z direction");
	inputSlots_[3]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[3]->SetDefaultValue(4);
	inputSlots_[3]->DefaultSet();

	inputSlots_[4]->SetName("size");
	inputSlots_[4]->SetVariableName("s");
	inputSlots_[4]->SetDescription("Size of cells");
	inputSlots_[4]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[4]->SetDefaultValue(1.0f);
	inputSlots_[4]->DefaultSet();

	outputSlots_[0]->SetName("Hexagonal Grid Vertices");
	outputSlots_[0]->SetVariableName("G");
	outputSlots_[0]->SetDescription("Vertices");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[1]->SetName("Hexagonal Grid Polylines");
	outputSlots_[1]->SetVariableName("P");
	outputSlots_[1]->SetDescription("Cell Polylines");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[1]->SetDataAccess(DataAccess::LIST);
}

void Maths_HexGrid::SolveInstance(
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
		// getting inputs
		Matrix3x4 trans = inSolveInstance[0].GetMatrix3x4();
		int x_numb = inSolveInstance[1].GetInt();
		int y_numb = inSolveInstance[2].GetInt();
		int z_numb = inSolveInstance[3].GetInt();

		// s is the side length of an equilateral hexagon
		float s = inSolveInstance[4].GetFloat();

		VariantVector gridList;


		// Set up vector of coordinates for the hexagons.
		// using "cube" coordinates
		// see this resource: 
		// http://stackoverflow.com/questions/2049196/generating-triangular-hexagonal-coordinates-xyz
		VariantVector coordinates;

		for (int i = -1 * x_numb; i < x_numb + 1; ++i)
		{
			for (int j = -1 * y_numb; j < y_numb + 1; ++j)
			{
				for (int k = -1 * z_numb; k < z_numb + 1; ++k)
				{
					if ((i + j + k == 0))
					{
						Vector3 tempVec = Vector3(i, j, k);
						coordinates.Push((Variant)tempVec);
					}
				}
			}
		}


		// Something is weird about this conversion
		// needed to add 1.5 multiplier to x basis, not sure why
		// looking at this: 
		// http://stackoverflow.com/questions/2459402/hexagonal-grid-coordinates-to-pixel-coordinates

		for (int i = 0; i < coordinates.Size(); ++i)
		{
			Vector3 curCoord = coordinates[i].GetVector3();
			float x = 1.5*s*(3 / 2)*curCoord.x_;
			float z = s*sqrt(3)*(curCoord.y_ + curCoord.x_ / 2);

			Vector3 coordOut = Vector3(x, 0.0f, z);
			gridList.Push((Variant)coordOut);
		}

		// cos(60)
		float c60 = 0.5;
		// sin(60)
		float s60 = sqrt(3) / 2;

		// make the polylines
		VariantVector polylineList;
		for (int i = 0; i < gridList.Size(); ++i)
		{
			VariantMap currHex; //polyline
			VariantVector verts;

			Vector3 centre = gridList[i].GetVector3();

			verts.Push(centre + Vector3(s, 0.0f, 0.0f));
			verts.Push(centre + Vector3(s*c60, 0.0f, s*s60));
			verts.Push(centre + Vector3(-1*s*c60, 0.0f, s*s60));
			verts.Push(centre + Vector3(-1* s, 0.0f, 0.0f));
			verts.Push(centre + Vector3(-1*s*c60, 0.0f, -1*s*s60));
			verts.Push(centre + Vector3(s*c60, 0.0f, -1*s*s60));
			verts.Push(centre + Vector3(s, 0.0f, 0.0f));

			//currHex["vertices"] = verts;
			polylineList.Push(Polyline_Make(verts));
		}


		//// now do the transformation to another basis
		VariantVector gridList_transformed;
		VariantVector polylineList_transformed;

		// transform the coordinates
		for (int i = 0; i < gridList.Size(); ++i)
		{
			Vector3 curCoord = gridList[i].GetVector3();
			Vector3 newCoord = trans*curCoord;
			gridList_transformed.Push(newCoord);
		}

		// transform the polylines
		for (int i = 0; i < polylineList.Size(); ++i)
		{
			VariantMap curPoly = polylineList[i].GetVariantMap();
			VariantVector newVertexList;
			VariantVector vertexList = Polyline_ComputeSequentialVertexList(polylineList[i]);

			// apply transform to vertex list
			for (unsigned i = 0; i < vertexList.Size(); ++i) {
				Vector3 vert = vertexList[i].GetVector3();
				Vector3 transformedVert = trans*vert;
				newVertexList.Push(Variant(transformedVert));
			}

			// newVertexList is ready
			
			polylineList_transformed.Push(Polyline_Make(newVertexList));
		}

		outSolveInstance[0] = gridList_transformed;
		outSolveInstance[1] = polylineList_transformed;
	}

}
