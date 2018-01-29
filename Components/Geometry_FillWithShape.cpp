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


#include "Geometry_FillWithShape.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Plane.h>

#include "TriMesh.h"
#include "Polyline.h"
#include "Geomlib_ConstructTransform.h"
#include "igl/ray_mesh_intersect.h"

using namespace Urho3D;
using namespace Eigen;
String Geometry_FillWithShape::iconTexture = "Textures/Icons/Geometry_FillWithShape.png";

namespace
{
	void Increment(Vector<int>& indices)
	{
		//subtract one from the first non-zero entry
		bool done = false;
		int counter = 0;

		while (counter < indices.Size())
		{
			if (indices[counter] > 0)
			{
				indices[counter] = indices[counter] - 1;
				break;
			}

			counter++;
		}
	}


	Vector<int> NumberToIndices(int a, int limit, int numRules)
	{
		Vector<int> indices;
		indices.Resize(numRules);
		for (int i = numRules; i > 0; i--)
		{
			indices[i] = FloorToInt(a / Pow(limit, i));
			a = a % Pow(limit, i);
		}

		return indices;
	}

	Matrix3x4 GetTransfromFromIndices(const Vector<int> indices, Vector<Matrix3x4> rules)
	{
		Matrix3x4 res = Matrix3x4::IDENTITY;
		String msg = "Indices: ";
		//first iteratively apply the rule at each index
		for (int i = 0; i < indices.Size(); i++)
		{

			//for (int j = 0; j < indices[i]; j++)
			//{
			rules[i] = indices[i] * rules[i];
			//}

			msg.AppendWithFormat("%d ", indices[i]);
		}

		//then concatenate
		for (int i = 0; i < rules.Size(); i++)
		{
			res = res + rules[i];
		}

		//msg.AppendWithFormat(", mat: %s", res.ToString().CString());

		URHO3D_LOGINFO(msg);

		//return
		return res;
	}
}


Geometry_FillWithShape::Geometry_FillWithShape(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("ProjectOnto");
	SetFullName("ProjectOnto");
	SetDescription("Projects geometry on to other geometry");

	AddInputSlot(
		"Source",
		"S",
		"Source geometry",
		VAR_VARIANTMAP,
		ITEM
	);

	AddInputSlot(
		"Transforms",
		"T",
		"Transforms",
		VAR_MATRIX3X4,
		LIST
	);

	AddInputSlot(
		"Iteration",
		"I",
		"Iterations",
		VAR_FLOAT,
		ITEM,
		3
	);

	AddInputSlot(
		"Base",
		"B",
		"Base transform",
		VAR_MATRIX3X4,
		ITEM,
		Matrix3x4::IDENTITY
	);

	AddInputSlot(
		"Target",
		"T",
		"Target geometry",
		VAR_VARIANTMAP,
		ITEM
	);

	AddOutputSlot(
		"Geo",
		"G",
		"Result geometry",
		VAR_VARIANTMAP,
		LIST
	);


}

void Geometry_FillWithShape::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{

	//check that we have the right types
	if (!TriMesh_Verify(inSolveInstance[0]) || !TriMesh_Verify(inSolveInstance[4]))
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//need some kind of safety
	int limit = inSolveInstance[2].GetInt();

	//construct global transform	
	Matrix3x4 base = Geomlib::ConstructTransform(inSolveInstance[3]);

	//apply scale to unit cell
	Variant unitCell = TriMesh_ApplyTransform(inSolveInstance[0], base);

	//get the transforms
	VariantVector xforms = inSolveInstance[1].GetVariantVector();
	Vector<Matrix3x4> rules;
	for (int i = 0; i < xforms.Size(); i++)
	{
		rules.Push(Geomlib::ConstructTransform(xforms[i]));
	}

	//loop over all rules up to iteration max and apply xform
	VariantVector cellsOut;
	int numRules = rules.Size();
	int total = Pow(limit, numRules);
	cellsOut.Resize(total);
	Matrix3x4 dx = Matrix3x4::IDENTITY;
	Vector<int> indices;

	//init the indices
	for (int i = 0; i < numRules; i++)
	{
		indices.Push(limit);
	}

	//now loop
	for (int i = 0; i < total; i++)
	{
		//update indices
		//i.e. cartesian product
		float idx = i;
		for (int j = 0; j < numRules; j++)
		{
			int dim = numRules - j - 1;
			int pow = Pow(limit, dim);
			int mod = FloorToInt((float)idx / pow);
			indices[j] = mod;
			idx = idx - mod*pow;
		}

		//what is the xform for this index
		dx = GetTransfromFromIndices(indices, rules);

		//apply it
		cellsOut[i] = TriMesh_ApplyTransform(unitCell, dx);
	}


	outSolveInstance[0] = cellsOut;

}
