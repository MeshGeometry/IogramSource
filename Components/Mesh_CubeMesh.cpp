#include "Mesh_CubeMesh.h"

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

String Mesh_CubeMesh::iconTexture = "Textures/Icons/Mesh_CubeMesh.png";

Mesh_CubeMesh::Mesh_CubeMesh(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("Cube");
	SetFullName("ConstructCubeMesh");
	SetDescription("Construct a cube mesh from scale");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("Side");
	inputSlots_[0]->SetVariableName("Side");
	inputSlots_[0]->SetDescription("Side length");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant(4.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Transformation");
	inputSlots_[1]->SetVariableName("T");
	inputSlots_[1]->SetDescription("Transformation to apply to cube");
	inputSlots_[1]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[1]->SetDefaultValue(Matrix3x4::IDENTITY);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Cube Mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_CubeMesh::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	VariantType type0 = inSolveInstance[0].GetType();
	if (!(type0 == VariantType::VAR_FLOAT || type0 == VariantType::VAR_INT)) {
		URHO3D_LOGWARNING("S must be a valid float or integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	float s = inSolveInstance[0].GetFloat();
	if (s <= 0.0f) {
		URHO3D_LOGWARNING("S must be > 0.");
		outSolveInstance[0] = Variant();
		return;
	}
	// Verify input slot 1
	VariantType type1 = inSolveInstance[1].GetType();
	if (type1 != VariantType::VAR_MATRIX3X4) {
		URHO3D_LOGWARNING("T must be a valid transform.");
		outSolveInstance[0] = Variant();
		return;
	}
	Matrix3x4 tr = inSolveInstance[1].GetMatrix3x4();

	///////////////////
	// COMPONENT'S WORK

	Variant baseCubeMesh = MakeCubeMesh(s);
	Variant cubeMesh = TriMesh_ApplyTransform(baseCubeMesh, tr);

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = cubeMesh;
}