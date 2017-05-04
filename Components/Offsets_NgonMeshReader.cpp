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


#include "Offsets_NgonMeshReader.h"

#include <iostream>

#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/File.h>

#include "NMesh.h"
#include "TriMesh.h"

using namespace Urho3D;

String Offsets_NgonMeshReader::iconTexture = "";

namespace {

Variant NMeshFromJSONValue(JSONValue& value)
{
	if (value["vertices"].IsNull() || value["faces"].IsNull()) {
		return Variant();
	}

	const JSONArray& vertsArr = value["vertices"].GetArray();
	VariantVector verts;
	for (int i = 0; i < vertsArr.Size(); i++)
	{
		const JSONArray& currVertArr = vertsArr[i].GetArray();
		if (currVertArr.Size() != 3) {
			continue;
		}

		float x = currVertArr[0].GetFloat();
		float y = currVertArr[1].GetFloat();
		float z = currVertArr[2].GetFloat();

		Vector3 vec(x, y, z);
		verts.Push(vec);
	}

	const JSONArray& facesArr = value["faces"].GetArray();
	VariantVector faces;
	for (int i = 0; i < facesArr.Size(); i++)
	{
		VariantVector indices_for_this_face;

		const JSONArray& indicesArr = facesArr[i]["indices"].GetArray();

		for (int j = 0; j < indicesArr.Size(); j++) {
			int index = indicesArr[j].GetInt();
			indices_for_this_face.Push(index);
		}

		int num_indices_for_this_face = indices_for_this_face.Size();
		faces.Push(Variant(num_indices_for_this_face));
		for (int j = 0; j < num_indices_for_this_face; j++) {
			faces.Push(Variant(indices_for_this_face[j].GetInt()));
		}
	}

	for (int i = 0; i < faces.Size(); ++i) {
		std::cout << faces[i].GetInt() << std::endl;
	}

	Variant nmesh = NMesh_Make(verts, faces);
	return nmesh;
}

} // namespace

Offsets_NgonMeshReader::Offsets_NgonMeshReader(Urho3D::Context* context) : IoComponentBase(context, 1, 3)
{
	SetName("OFFSETS_NgonMeshReader");
	SetFullName("OFFSETS_NgonMeshReader");
	SetDescription("Offsets NgonMesh reader");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Path");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Path to grain JSON file");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("NMesh");
	outputSlots_[0]->SetVariableName("N");
	outputSlots_[0]->SetDescription("NMesh for grain geometry");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("TriMesh");
	outputSlots_[1]->SetVariableName("T");
	outputSlots_[1]->SetDescription("TriMesh for grain geometry");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[2]->SetName("FacePolylines");
	outputSlots_[2]->SetVariableName("P");
	outputSlots_[2]->SetDescription("Polylines of NMesh faces");
	outputSlots_[2]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[2]->SetDataAccess(DataAccess::LIST);
}

void Offsets_NgonMeshReader::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Variant path_var = inSolveInstance[0];
	if (!path_var.GetType() == VariantType::VAR_STRING) {
		URHO3D_LOGWARNING("P path must be stored as valid String");
		outSolveInstance[0] = Variant();
		return;
	}

	FileSystem* fs = GetSubsystem<FileSystem>();
	String path = path_var.GetString();

	if (fs->FileExists(path)) {

		File* f = new File(GetContext(), path);
		JSONFile* jFile = new JSONFile(GetContext());
		jFile->Load(*f);

		JSONValue& root = jFile->GetRoot();

		Variant meshOut = NMeshFromJSONValue(root);

		outSolveInstance[0] = meshOut;
		Variant tri_mesh_out = NMesh_ConvertToTriMesh(meshOut);
		outSolveInstance[1] = tri_mesh_out;
		outSolveInstance[2] = Variant(NMesh_ComputeWireframePolylines(meshOut));

		//outSolveInstance[0] = Variant();

	}
	else {
		URHO3D_LOGWARNING("Uh oh it's length 0");
		outSolveInstance[0] = Variant();
	}
}