#include "Mesh_AverageEdgeLength.h"

#include <Urho3D/Core/Variant.h>

#include "Geomlib_TriMeshAverageEdgeLength.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_AverageEdgeLength::iconTexture = "";

Mesh_AverageEdgeLength::Mesh_AverageEdgeLength(Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("AvgEdge");
	SetFullName("Average Edge Length");
	SetDescription("Compute average edge length for TriMesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("MeshIn");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh to compute average edge length on");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("Length");
	outputSlots_[0]->SetVariableName("L");
	outputSlots_[0]->SetDescription("Average edge length");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_AverageEdgeLength::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Variant mesh_in = inSolveInstance[0];
	if (!TriMesh_Verify(mesh_in)) {
		URHO3D_LOGWARNING("M must be a TriMesh!");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	float length = Geomlib::TriMeshAverageEdgeLength(mesh_in);

	outSolveInstance[0] = length;
}