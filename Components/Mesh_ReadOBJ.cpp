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

#include "Mesh_ReadOBJ.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "Geomlib_ReadOBJ.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_ReadOBJ::iconTexture = "Textures/Icons/Mesh_ReadOBJ.png";

Mesh_ReadOBJ::Mesh_ReadOBJ(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("ReadOBJ");
	SetFullName("ReadOBJ");
	SetDescription("Read TriMesh from OBJ file");

	inputSlots_[0]->SetName("File");
	inputSlots_[0]->SetVariableName("File");
	inputSlots_[0]->SetDescription("File");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("ToYUp");
	inputSlots_[1]->SetVariableName("ToYUp");
	inputSlots_[1]->SetDescription("Transform coords to Y Up");
	inputSlots_[1]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(false);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_ReadOBJ::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	String filename = inSolveInstance[0].GetString();
	if (filename.Empty()) {
		URHO3D_LOGERROR("Mesh_ReadOBJ --- invalid File");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	SharedPtr<FileSystem> fs(GetSubsystem<FileSystem>());
	bool cached = false;
	SharedPtr<File> obj_file = GetSubsystem<ResourceCache>()->GetFile(filename);
	if (obj_file) {
		cached = true;
	}

	bool yup = inSolveInstance[1].GetBool();

	Variant tri_mesh;

	bool success;
	if (cached) {
		URHO3D_LOGINFO("Mesh_ReadOBJ --- attempting to read OBJ file from cache");
		success = Geomlib::ReadOBJ(obj_file, tri_mesh, yup);
	}
	else {
		success = Geomlib::ReadOBJ(filename, tri_mesh, yup);
	}

	if (!success) {
		URHO3D_LOGERROR("Mesh_ReadOBJ --- Geomlib::ReadOBJ failed");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	outSolveInstance[0] = tri_mesh;
}