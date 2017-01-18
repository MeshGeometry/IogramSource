#include "Mesh_ExtrudePolyline.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "Mesh_ExtrudePolyline.h"

#include "TriMesh.h"

using namespace Urho3D;

String Mesh_ExtrudePolyline::iconTexture = "Textures/Icons/Mesh_ExtrudePolyline.png";

Mesh_ExtrudePolyline::Mesh_ExtrudePolyline(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("ExtrudePolyline");
	SetFullName("Extrude Polyline");
	SetDescription("Extrudes a polyline along a vector");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Polyline");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Polyline to extrude");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Extrude Direction");
	inputSlots_[1]->SetVariableName("V");
	inputSlots_[1]->SetDescription("Vector along which to extrude");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Vector3(0,1,0));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Framed mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_ExtrudePolyline::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	bool dataCheck = false;
	if (inSolveInstance[0].GetType() == VAR_VARIANTMAP)
	{
		if (inSolveInstance[0].GetVariantMap().Keys().Contains("vertices"))
			dataCheck = true;
	}

	if (!dataCheck)
	{
		return;
	}

	VariantMap poly = inSolveInstance[0].GetVariantMap();
	VariantVector verts = poly["vertices"].GetVariantVector(); //we will push new vertices to this vector
	VariantVector faces;
	int vertOffset = verts.Size();
	//extrusion direction
	Vector3 dirVec = inSolveInstance[1].GetVector3();
	float endPointDistance = (verts[0].GetVector3() - verts[vertOffset - 1].GetVector3()).Length();
	bool isClosed = (endPointDistance < 1.0f / M_LARGE_VALUE) ? true : false;

	//itr num
	int itrNum = vertOffset - 1;

	//if closed, delete the extra point at end of polyline
	if (isClosed)
	{
		verts.Erase(vertOffset - 1, 1);
		vertOffset = verts.Size();
		itrNum = vertOffset;
	}

	//push new vertices
	for (int i = 0; i < vertOffset; i++)
	{
		verts.Push(verts[i].GetVector3() + dirVec);
	}

	//create the faces
	for (int i = 0; i < itrNum; i++)
	{
		int a, b, c;

		//first triangle
		a = i;
		b = i + vertOffset;
		c = (i + 1) % vertOffset; 

		faces.Push(a); faces.Push(b); faces.Push(c);

		//second triangle
		a = (i + 1) % vertOffset;
		b = i + vertOffset;
		c = (i + 1) % vertOffset + vertOffset;

		faces.Push(a); faces.Push(b); faces.Push(c);
	}

	outSolveInstance[0] = TriMesh_Make(verts, faces);
}