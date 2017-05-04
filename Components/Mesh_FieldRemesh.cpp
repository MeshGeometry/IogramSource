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


#include "Mesh_FieldRemesh.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#pragma warning(push, 0)
#include <igl/writeOBJ.h>
#include <igl/readOBJ.h>
#include <igl/polygon_mesh_to_triangle_mesh.h>
#pragma warning(pop)

#include "ConversionUtilities.h"
#include "TriMesh.h"

// #include "common.h"
// #include "meshio.h"
// #include "dedge.h"
// #include "subdivide.h"
// #include "meshstats.h"
// #include "hierarchy.h"
// #include "field.h"
// #include "normal.h"
// #include "extract.h"
// #include "bvh.h"

using namespace Urho3D;

String Mesh_FieldRemesh::iconTexture = "";

Mesh_FieldRemesh::Mesh_FieldRemesh(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("FieldRemesh");
	SetFullName("FieldRemesh");
	SetDescription("Remeshes in a really clever and fast way (https://github.com/wjakob/instant-meshes)");

	AddInputSlot(
		"Mesh",
		"M",
		"Mesh to remesh",
		VAR_VARIANTMAP,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Faces",
		"N",
		"Number of faces",
		VAR_FLOAT,
		DataAccess::ITEM
	);

	inputSlots_[1]->SetDefaultValue(500);

	AddOutputSlot(
		"Mesh",
		"R",
		"Remeshed",
		VAR_VARIANTMAP,
		DataAccess::ITEM
	);
}

void Mesh_FieldRemesh::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	
}