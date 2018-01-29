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


#include "Mesh_JoinMeshes.h"

#include <assert.h>

#include <iostream>
#include <vector>

#include <Urho3D/Core/Variant.h>

#include "ConversionUtilities.h"
#include "TriMesh.h"
#include "NMesh.h"
#include "Geomlib_JoinMeshes.h"
#include "Geomlib_RemoveDuplicates.h"

using namespace Urho3D;

String Mesh_JoinMeshes::iconTexture = "Textures/Icons/Mesh_JoinMeshes.png";

Mesh_JoinMeshes::Mesh_JoinMeshes(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("JoinMeshes");
	SetFullName("Join Meshes");
	SetDescription("Join meshes in a list into a single mesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("MeshList");
	inputSlots_[0]->SetVariableName("ML");
	inputSlots_[0]->SetDescription("List of meshes");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);

	inputSlots_[1]->SetName("WeldDuplicates");
	inputSlots_[1]->SetVariableName("W");
	inputSlots_[1]->SetDescription("Join duplicate verts");
	inputSlots_[1]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(false);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("JoinMesh");
	outputSlots_[0]->SetVariableName("JM");
	outputSlots_[0]->SetDescription("Joined mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_JoinMeshes::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	Variant mesh_list_var = inSolveInstance[0];
	if (mesh_list_var.GetType() != VariantType::VAR_VARIANTVECTOR) {
		URHO3D_LOGWARNING("ML must be a list");
		outSolveInstance[0] = Variant();
		return;
	}
	VariantVector mesh_list = mesh_list_var.GetVariantVector();
	VariantVector actual_mesh_list;
	VariantVector actual_nmesh_list;
	for (int i = 0; i < mesh_list.Size(); ++i) {
		if (TriMesh_Verify(mesh_list[i])) {
			actual_mesh_list.Push(mesh_list[i]);
		}
		else if (NMesh_Verify(mesh_list[i])) {
			actual_nmesh_list.Push(mesh_list[i]);
		}
	}

	if (actual_mesh_list.Size() == 0 && actual_nmesh_list.Size() == 0) {
		URHO3D_LOGWARNING("ML contains no valid meshes, skipping JoinMeshes operation");
		outSolveInstance[0] = Variant();
		return;
	}
	bool weld = inSolveInstance[1].GetBool();

	///////////////////
	// COMPONENT'S WORK

	if (actual_mesh_list.Size() > 0) {
		Variant joined_mesh = Geomlib::JoinMeshes(actual_mesh_list);
		if (!weld)
			outSolveInstance[0] = joined_mesh;
		else {
			VariantVector verts = TriMesh_GetVertexList(joined_mesh);
			VariantVector faces = TriMesh_GetFaceList(joined_mesh);
			Vector<Vector3> vertexCoords;

			for (int i = 0; i < faces.Size(); ++i) {
				int id = faces[i].GetInt();
				Vector3 curVert = verts[id].GetVector3();
				vertexCoords.Push(curVert);
			}
			Variant m_vertices, m_faces;
			Geomlib::RemoveDuplicates(vertexCoords, m_vertices, m_faces);
			Variant mergeMesh = TriMesh_Make(m_vertices, m_faces);
			outSolveInstance[0] = mergeMesh;
		}
	}
	else if (actual_nmesh_list.Size() > 0) {
		Variant joined_nmesh = Geomlib::JoinNMeshes(actual_nmesh_list);
		if (!weld)
			outSolveInstance[0] = joined_nmesh;
		else {
			VariantVector verts = NMesh_GetVertexList(joined_nmesh);
			VariantVector structuredFaceList = NMesh_GetFaceList(joined_nmesh);
			Vector<Vector3> vertexCoords;

			for (int i = 0; i < structuredFaceList.Size(); ++i) {
				Urho3D::VariantMap face = structuredFaceList[i].GetVariantMap();
				Urho3D::VariantVector face_verts = face["face_vertices"].GetVariantVector();
				for (int k = 0; k < face_verts.Size(); ++k) {
					int id = face_verts[k].GetInt();
					Vector3 curVert = verts[id].GetVector3();
					vertexCoords.Push(curVert);
				}
				
			}
			Variant m_vertices, m_faces;
			Geomlib::NMesh_RemoveDuplicates(vertexCoords, structuredFaceList, m_vertices, m_faces);
			Variant mergeMesh = NMesh_Make(m_vertices.GetVariantVector(), m_faces.GetVariantVector());
			outSolveInstance[0] = mergeMesh;
		}
	}
	else
		outSolveInstance[0] = Variant();

	/////////////////
	// ASSIGN OUTPUTS


	//outSolveInstance[0] = joined_mesh;
}

