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


#include "Maths_RectangularArray.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_RectangularArray::iconTexture = "Textures/Icons/Maths_RectangularArray.png";

Maths_RectangularArray::Maths_RectangularArray(Urho3D::Context* context) : IoComponentBase(context, 7, 1)
{
	SetName("RectangularArray");
	SetFullName("Rectangular Array");
	SetDescription("Generates a 3D rectangular array of cells from transform, number of cells and cell size");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("transform");
	inputSlots_[0]->SetVariableName("T");
	inputSlots_[0]->SetDescription("Transform for coordinate system");
	inputSlots_[0]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[0]->SetDefaultValue(Matrix3x4::IDENTITY);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("numberX");
	inputSlots_[1]->SetVariableName("NX");
	inputSlots_[1]->SetDescription("Number of cells in X direction");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDefaultValue(10);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("numberY");
	inputSlots_[2]->SetVariableName("NY");
	inputSlots_[2]->SetDescription("Number of cells in Y direction");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDefaultValue(10);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("numberZ");
	inputSlots_[3]->SetVariableName("NZ");
	inputSlots_[3]->SetDescription("Number of cells in Z direction");
	inputSlots_[3]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[3]->SetDefaultValue(10);
	inputSlots_[3]->DefaultSet();

	inputSlots_[4]->SetName("sizeX");
	inputSlots_[4]->SetVariableName("x");
	inputSlots_[4]->SetDescription("Size of cells in X direction");
	inputSlots_[4]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[4]->SetDefaultValue(1.0f);
	inputSlots_[4]->DefaultSet();

	inputSlots_[5]->SetName("sizeY");
	inputSlots_[5]->SetVariableName("y");
	inputSlots_[5]->SetDescription("size of cells in Y direction");
	inputSlots_[5]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[5]->SetDefaultValue(1.0f);
	inputSlots_[5]->DefaultSet();

	inputSlots_[6]->SetName("sizeZ");
	inputSlots_[6]->SetVariableName("z");
	inputSlots_[6]->SetDescription("size of cells in Z direction");
	inputSlots_[6]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[6]->SetDefaultValue(1.0f);
	inputSlots_[6]->DefaultSet();

	outputSlots_[0]->SetName("Rectangular Grid Vertices");
	outputSlots_[0]->SetVariableName("G");
	outputSlots_[0]->SetDescription("Vertices");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	//outputSlots_[1]->SetName("Rectangular Grid Polylines");
	//outputSlots_[1]->SetVariableName("P");
	//outputSlots_[1]->SetDescription("Cell Polylines");
	//outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	//outputSlots_[1]->SetDataAccess(DataAccess::LIST);
}

void Maths_RectangularArray::SolveInstance(
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
		(inSolveInstance[4].GetType() == VAR_INT || inSolveInstance[4].GetType() == VAR_FLOAT) &&
		(inSolveInstance[5].GetType() == VAR_INT || inSolveInstance[5].GetType() == VAR_FLOAT) &&
		(inSolveInstance[6].GetType() == VAR_INT || inSolveInstance[6].GetType() == VAR_FLOAT) )
	{
		res = true;
	}

	if (res)
	{

		// get the rectangular grid as in plane component
		Matrix3x4 trans = inSolveInstance[0].GetMatrix3x4();
		int x_numb = inSolveInstance[1].GetInt();
		int y_numb = inSolveInstance[2].GetInt();
		int z_numb = inSolveInstance[3].GetInt();
		float x_size = inSolveInstance[4].GetFloat();
		float y_size = inSolveInstance[5].GetFloat();
		float z_size = inSolveInstance[6].GetFloat();


		// Set up x-, y-, z- array "basis" vectors, incorporating transform information
		Vector3 origin = Vector3::ZERO;
		Vector3 x_basis = Vector3(x_size, 0.0f, 0.0f);
		Vector3 y_basis = Vector3(0.0f, y_size, 0.0f);
		Vector3 z_basis = Vector3(0.0f, 0.0f, z_size);

		VariantVector gridList;
		VariantVector arrayList;

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

		// Add grids in y-direction
		for (int i = 0; i < y_numb + 1; ++i)
		{
			VariantVector currGrid;
			for (int j = 0; j < gridList.Size(); ++j)
			{
				currGrid.Push(Variant(gridList[j].GetVector3() + i*y_basis));
			}
			arrayList.Push(currGrid);
		}

		// now transform everything
		VariantVector arrayList_transformed;

		// transform the coordinates of the cell corners
		for (int i = 0; i < arrayList.Size(); ++i)
		{
			Vector3 curCoord = arrayList[i].GetVector3();
			Vector3 newCoord = trans*curCoord;
			arrayList_transformed.Push(Variant(newCoord));
		}


		outSolveInstance[0] = arrayList_transformed;

	}
	else
	{ // !res
		URHO3D_LOGWARNING("Bad inputs! Review input types.");
		outSolveInstance[0] = Variant();
	}

}
