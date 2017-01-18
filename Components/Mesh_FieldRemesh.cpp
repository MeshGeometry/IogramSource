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