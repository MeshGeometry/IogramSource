#include "Mesh_Torus.h"

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

String Mesh_Torus::iconTexture = "Textures/Icons/Mesh_Torus.png";

Mesh_Torus::Mesh_Torus(Context* context) : IoComponentBase(context, 5, 1)
{
	SetName("Torus");
	SetFullName("ConstructTorusMesh");
	SetDescription("Construct a torus mesh from radii");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("OuterRadius");
	inputSlots_[0]->SetVariableName("O");
	inputSlots_[0]->SetDescription("Outer radius");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant(4.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("InnerRadius");
	inputSlots_[1]->SetVariableName("I");
	inputSlots_[1]->SetDescription("Inner radius");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(1.0f));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("FirstPower");
	inputSlots_[2]->SetVariableName("P1");
	inputSlots_[2]->SetDescription("FirstPower");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(Variant(1.0f));
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("SecondPower");
	inputSlots_[3]->SetVariableName("P2");
	inputSlots_[3]->SetDescription("SecondPower");
	inputSlots_[3]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(Variant(1.0f));
	inputSlots_[3]->DefaultSet();

	inputSlots_[4]->SetName("MeshResolution");
	inputSlots_[4]->SetVariableName("R");
	inputSlots_[4]->SetDescription("Integer (>3) describing mesh resolution");
	inputSlots_[4]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[4]->SetDefaultValue(Variant(8));
	inputSlots_[4]->DefaultSet();

	//inputSlots_[4]->SetName("Transformation");
	//inputSlots_[4]->SetVariableName("T");
	//inputSlots_[4]->SetDescription("Transformation to apply to cube");
	//inputSlots_[4]->SetVariantType(VariantType::VAR_MATRIX3X4);
	//inputSlots_[4]->SetDefaultValue(Matrix3x4::IDENTITY);
	//inputSlots_[4]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Torus Mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_Torus::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT
	float outer = inSolveInstance[0].GetFloat();
	outer = Max(0.0f, outer);
	float inner = inSolveInstance[1].GetFloat();
	inner = Max(0.0f, inner);
	float p_1 = inSolveInstance[2].GetFloat();
	float p_2 = inSolveInstance[3].GetFloat();
	int res = inSolveInstance[4].GetInt();
	res = Max(3, res);

	///////////////////
	// COMPONENT'S WORK

	Variant torusMesh = MakeSuperTorus(outer, inner, p_1, p_2, res);

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = torusMesh;
}
