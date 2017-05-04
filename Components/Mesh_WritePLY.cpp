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

#include "Mesh_WritePLY.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "Geomlib_WritePLY.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_WritePLY::iconTexture = "Textures/Icons/Mesh_WritePLY.png";

Mesh_WritePLY::Mesh_WritePLY(Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("WritePLY");
	SetFullName("WritePLY");
	SetDescription("Write TriMesh to PLY file");

	inputSlots_[0]->SetName("FileName");
	inputSlots_[0]->SetVariableName("FileName");
	inputSlots_[0]->SetDescription("FileName");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Mesh");
	inputSlots_[1]->SetVariableName("M");
	inputSlots_[1]->SetDescription("Mesh");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[2]->SetName("ToZUp");
	inputSlots_[2]->SetVariableName("ToZUp");
	inputSlots_[2]->SetDescription("Transform coords to Z Up");
	inputSlots_[2]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(false);
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("SavedName");
	outputSlots_[0]->SetVariableName("SavedName");
	outputSlots_[0]->SetDescription("SavedName");
	outputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_WritePLY::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	String filename = inSolveInstance[0].GetString();
	if (filename.Empty()) {
		URHO3D_LOGERROR("Mesh_WritePLY --- invalid FileName");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	int len = filename.Length();
	if (
		len < 4 ||
		filename.Substring(len - 4) != String(".ply")
		)
	{
		filename += ".ply";
	}

	Variant tri_mesh = inSolveInstance[1];
	if (!TriMesh_Verify(tri_mesh)) {
		URHO3D_LOGERROR("Mesh_WritePLY --- invalid Mesh input");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	bool zup = inSolveInstance[2].GetBool();

	bool success = Geomlib::WritePLY(filename, tri_mesh, zup);
	if (!success) {
		URHO3D_LOGERROR("Mesh_WritePLY --- Geomlib::WritePLY failed");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	outSolveInstance[0] = filename;
}