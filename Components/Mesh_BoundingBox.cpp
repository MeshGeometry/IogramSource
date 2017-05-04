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


#include "Mesh_BoundingBox.h"

#include <Urho3D/Core/Variant.h>

#include "TriMesh.h"

using namespace Urho3D;

String Mesh_BoundingBox::iconTexture = "";

Mesh_BoundingBox::Mesh_BoundingBox(Context* context) : IoComponentBase(context, 1, 5)
{
	SetName("BoundingBox");
	SetFullName("Bounding Box");
	SetDescription("Construct TriMesh bounding box for another TriMesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("BoxMesh");
	outputSlots_[0]->SetVariableName("B");
	outputSlots_[0]->SetDescription("Meshed bounding box of input mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("BoxMin");
	outputSlots_[1]->SetVariableName("min");
	outputSlots_[1]->SetDescription("Minimum corner");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[2]->SetName("BoxMax");
	outputSlots_[2]->SetVariableName("max");
	outputSlots_[2]->SetDescription("maximum corner");
	outputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[2]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[3]->SetName("BoxCenter");
	outputSlots_[3]->SetVariableName("C");
	outputSlots_[3]->SetDescription("Centre of box");
	outputSlots_[3]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[3]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[4]->SetName("Diagonal");
	outputSlots_[4]->SetVariableName("D");
	outputSlots_[4]->SetDescription("Length of box diagonal");
	outputSlots_[4]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[4]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_BoundingBox::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Variant mesh_in = inSolveInstance[0];
	if (!TriMesh_Verify(mesh_in)) {
		URHO3D_LOGWARNING("M must be a TriMesh!");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Variant box_mesh = TriMesh_BoundingBox(mesh_in);

	// find the min and max

	VariantVector vertexList = TriMesh_GetVertexList(box_mesh);
	Vector3 min = vertexList[0].GetVector3();
	Vector3 max = vertexList[0].GetVector3();
	for (int i = 0; i < vertexList.Size(); ++i) {
		Vector3 curVert = vertexList[i].GetVector3();
		if (curVert.x_ <= min.x_ && curVert.y_ <= min.y_ && curVert.z_ <= min.z_)
			min = curVert;
		if (curVert.x_ >= max.x_ && curVert.y_ >= max.y_ && curVert.z_ >= max.z_)
			max = curVert;
	}

	Vector3 centre = (max + min) / 2;
	float diag = (max - min).Length();

	outSolveInstance[0] = box_mesh;
	outSolveInstance[1] = Variant(min);
	outSolveInstance[2] = Variant(max);
	outSolveInstance[3] = Variant(centre);
	outSolveInstance[4] = Variant(diag);

}