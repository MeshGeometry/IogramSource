#include "Vector_DeconstructVector.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>

using namespace Urho3D;

String Vector_DeconstructVector::iconTexture = "Textures/Icons/Vector_DeconstructVector.png";

Vector_DeconstructVector::Vector_DeconstructVector(Context* context) :
	IoComponentBase(context, 1, 3)
{
	SetName("DecVec");
	SetFullName("Deconstruct Vector");
	SetDescription("Deconstruct a vector into its components");
	SetGroup(IoComponentGroup::VECTOR);
	SetSubgroup("Vector");

	inputSlots_[0]->SetName("Vector");
	inputSlots_[0]->SetVariableName("V");
	inputSlots_[0]->SetDescription("Vector to deconstruct");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Vector3(0.0f, 0.0f, 0.0f));
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("X-Coordinate");
	outputSlots_[0]->SetVariableName("X");
	outputSlots_[0]->SetDescription("X-Coordinate of vector");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Y-Coordinate");
	outputSlots_[1]->SetVariableName("Y");
	outputSlots_[1]->SetDescription("Y-Coordinate of vector");
	outputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[2]->SetName("Z-Coordinate");
	outputSlots_[2]->SetVariableName("Z");
	outputSlots_[2]->SetDescription("Z-Coordinate of vector");
	outputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[2]->SetDataAccess(DataAccess::ITEM);
}

void Vector_DeconstructVector::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	Variant xVariant, yVariant, zVariant;

	///////////////////
	// EXTRACT & VERIFY

	if (inSolveInstance[0].GetType() != VariantType::VAR_VECTOR3) {
		URHO3D_LOGWARNING("V must be a valid Vector3");
		outSolveInstance[0] = Variant();
		outSolveInstance[1] = Variant();
		outSolveInstance[2] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Variant inVariant = inSolveInstance[0];
	Vector3 vec = inVariant.GetVector3();
	xVariant = Variant(vec.x_);
	yVariant = Variant(vec.y_);
	zVariant = Variant(vec.z_);

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = xVariant;
	outSolveInstance[1] = yVariant;
	outSolveInstance[2] = zVariant;
}