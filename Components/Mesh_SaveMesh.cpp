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


#include "Mesh_SaveMesh.h"
#include "Geomlib_TriMeshSaveOFF.h"

using namespace Urho3D;

String Mesh_SaveMesh::iconTexture = "Textures/Icons/Mesh_SaveMesh.png";

Mesh_SaveMesh::Mesh_SaveMesh(Context* context) :IoComponentBase(context, 0, 0)
{
	SetName("SaveMesh");
	SetFullName("Save Mesh");
	SetDescription("Saves a mesh in a variety of formats");

	AddInputSlot(
		"Mesh",
		"M",
		"Mesh to save",
		VAR_VARIANTMAP,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Path",
		"P",
		"Path",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Result",
		"R",
		"Result",
		VAR_BOOL,
		DataAccess::ITEM
	);
}

void Mesh_SaveMesh::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	VariantMap mesh = inSolveInstance[0].GetVariantMap();

	if (mesh.Keys().Contains("vertices") && mesh.Keys().Contains("faces"))
	{
		String path = inSolveInstance[1].GetString();
		if (path.Empty())
		{
			outSolveInstance[0] = Variant();
			return;
		}

		File* file = new File(GetContext());
		file->Open(path, FileMode::FILE_WRITE);

		bool result = Geomlib::TriMeshWriteOFF(mesh, *file);

		outSolveInstance[0] = result;

		file->Close();
	}
	else
	{
		outSolveInstance[0] = Variant();
		return;
	}
}


