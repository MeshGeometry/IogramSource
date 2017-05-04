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


#include "Scene_DeconstructTransform.h"

#include "Scene_DeconstructTransform.h"

#include <assert.h>

using namespace Urho3D;

String Scene_DeconstructTransform::iconTexture = "Textures/Icons/Scene_DeconstructTransform.png";

Scene_DeconstructTransform::Scene_DeconstructTransform(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("DeconstructTransform");
	SetFullName("Deconstruct Transform");
	SetDescription("Deconstruct a transform into position, rotation, and scale");

	AddInputSlot(
		"Transform",
		"T",
		"Transform to deconstruct",
		VAR_MATRIX3X4,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Position",
		"P",
		"Position",
		VAR_VECTOR3,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Scale",
		"S",
		"Scale",
		VAR_VECTOR3,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Rotation",
		"R",
		"Rotation",
		VAR_QUATERNION,
		DataAccess::ITEM
	);

}

void Scene_DeconstructTransform::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Variant matVar = inSolveInstance[0];
	if (matVar.GetType() != VAR_MATRIX3X4)
	{
		URHO3D_LOGERROR("Expected a matrix, received a: " + inSolveInstance[0].GetTypeName());
		outSolveInstance[0] = Variant();
		outSolveInstance[1] = Variant();
		outSolveInstance[2] = Variant();
		return;
	}

	Matrix3x4 mat = matVar.GetMatrix3x4();
	outSolveInstance[0] = mat.Translation();
	outSolveInstance[1] = mat.Scale();
	outSolveInstance[2] = mat.Rotation();

}