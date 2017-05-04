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


#include "Maths_RhodoArray3D.h"

#include <assert.h>

#include <iostream>

#include <Urho3D/Core/Variant.h>

#include "Geomlib_Rhodo.h"
#include "Geomlib_TransformVertexList.h"
#include "Polyline.h"

using namespace Urho3D;

String Maths_RhodoArray3D::iconTexture = "";

Maths_RhodoArray3D::Maths_RhodoArray3D(Urho3D::Context* context) : IoComponentBase(context, 5, 1)
{
	SetName("RhodoArray3D");
	SetFullName("Rhodo Array 3D");
	SetDescription("Generates 3D array of lattice points from rhombic dodecahedral tiling");
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
	inputSlots_[1]->SetDescription("Approx. width in rhodo diameters along x-axis of box");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDefaultValue(4);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("numberY");
	inputSlots_[2]->SetVariableName("NY");
	inputSlots_[2]->SetDescription("Approx. width in rhodo diameters along y-axis of box");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDefaultValue(4);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("numberZ");
	inputSlots_[3]->SetVariableName("NZ");
	inputSlots_[3]->SetDescription("Approx. width in rhodo diameters along z-axis of box");
	inputSlots_[3]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[3]->SetDefaultValue(4);
	inputSlots_[3]->DefaultSet();

	inputSlots_[4]->SetName("Diameter");
	inputSlots_[4]->SetVariableName("D");
	inputSlots_[4]->SetDescription("Diameter of one rhombic dodecahedron");
	inputSlots_[4]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[4]->SetDefaultValue(4.0f);
	inputSlots_[4]->DefaultSet();

	outputSlots_[0]->SetName("Rectangular Grid Vertices");
	outputSlots_[0]->SetVariableName("G");
	outputSlots_[0]->SetDescription("Vertices");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);
}

void Maths_RhodoArray3D::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	Matrix3x4 trans = inSolveInstance[0].GetMatrix3x4();
	int xNum = inSolveInstance[1].GetInt();
	int yNum = inSolveInstance[2].GetInt();
	int zNum = inSolveInstance[3].GetInt();
	float rhdiam = inSolveInstance[4].GetFloat();
	float scale = 1.0f;
	if (rhdiam > 0.0f) {
		scale = 0.25f * rhdiam;
	}

	VariantVector vertexList;

	for (int f = 0; f < 4; ++f) {
		for (int i = 0; i < xNum; ++i) {
			for (int j = 0; j < yNum; ++j) {
				for (int k = 0; k < zNum; ++k) {
					Geomlib::Rhodo rh(f, i, j, k);
					VariantVector rhVerts = rh.ComputeVertices(scale);
					vertexList.Push(rhVerts);
				}
			}
		}
	}

	/*
	VariantVector transVertexList;
	for (unsigned i = 0; i < vertexList.Size(); ++i) {
		Vector3 transVert = trans * vertexList[i].GetVector3();
		transVertexList.Push(transVert);
	}
	*/

	VariantVector transVertexList = Geomlib::TransformVertexList(trans, vertexList);


	outSolveInstance[0] = Polyline_Make(transVertexList);
}