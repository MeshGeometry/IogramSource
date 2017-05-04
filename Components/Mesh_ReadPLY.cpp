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

#include "Mesh_ReadPLY.h"

#include <assert.h>

#include <iostream>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "Geomlib_ReadPLY.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_ReadPLY::iconTexture = "Textures/Icons/Mesh_ReadPLY.png";

Mesh_ReadPLY::Mesh_ReadPLY(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("ReadPLY");
	SetFullName("ReadPLY");
	SetDescription("Read TriMesh from PLY file");

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

void Mesh_ReadPLY::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	String filename = inSolveInstance[0].GetString();
	if (filename.Empty()) {
		URHO3D_LOGERROR("Mesh_ReadPLY --- invalid File");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	SharedPtr<FileSystem> fs(GetSubsystem<FileSystem>());
	bool cached = false;
	SharedPtr<File> ply_file = GetSubsystem<ResourceCache>()->GetFile(filename);
	if (ply_file) {
		cached = true;
	}
	bool cached_ok = false;
#ifdef WIN32
	cached_ok = true;
#endif
#ifdef __APPLE__
	std::cout << "__APPLE__ is defined!" << std::endl;
	cached_ok = true;
#endif

	bool yup = inSolveInstance[1].GetBool();

	Variant tri_mesh;

	bool success;
	if (cached && cached_ok) {
		URHO3D_LOGINFO("Mesh_ReadPLY --- attempting to read PLY file from cache");
		success = Geomlib::ReadPLY(ply_file, tri_mesh, yup);
		if (success) {
			URHO3D_LOGINFO("Mesh_ReadPLY --- partial success reading PLY from cache");
		}
	}
	else {
		success = Geomlib::ReadPLY(filename, tri_mesh, yup);
	}

	if (!success) {
		URHO3D_LOGERROR("Mesh_ReadPLY --- Geomlib::ReadPLY failed");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	outSolveInstance[0] = tri_mesh;
}