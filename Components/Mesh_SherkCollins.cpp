#include "Mesh_SherkCollins.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ConversionUtilities.h"
#include "StockGeometries.h"
#include "Geomlib_TransformVertexList.h"
#include "TriMesh.h"
#include "Geomlib_TriMeshThicken.h"

using namespace Urho3D;

String Mesh_SherkCollins::iconTexture = "Textures/Icons/Mesh_SherkCollins.png";

namespace
{

};

Mesh_SherkCollins::Mesh_SherkCollins(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("SherkCollins");
	SetFullName("SherkCollins");
	SetDescription("Construct a Sherk Collins mesh from radii");

	AddInputSlot(
		"Storeys",
		"ST",
		"Storeys",
		VAR_INT,
		DataAccess::ITEM,
		2
		);

	AddInputSlot(
		"Mesh",
		"M",
		"Mesh",
		VAR_VARIANTMAP,
		DataAccess::ITEM
		);

}

void Mesh_SherkCollins::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{

}
