#include "Maths_Multiplication.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_Multiplication::iconTexture = "Textures/Icons/Maths_Multiplication.png";

Maths_Multiplication::Maths_Multiplication(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("X * Y");
	SetFullName("Multiplication");
	SetDescription("Mathematical multiplication");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("X");
	inputSlots_[0]->SetVariableName("X");
	inputSlots_[0]->SetDescription("First number to multiply");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant(0.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Y");
	inputSlots_[1]->SetVariableName("Y");
	inputSlots_[1]->SetDescription("Second number to multiply");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(0.0f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Product");
	outputSlots_[0]->SetVariableName("P");
	outputSlots_[0]->SetDescription("Product of the numbers");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Maths_Multiplication::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	Variant factor0 = inSolveInstance[0];
	Variant factor1 = inSolveInstance[1];

	VariantType type0 = factor0.GetType();
	VariantType type1 = factor1.GetType();

	Variant product;

	////////////////////////////////////////////////////////////
	if (
		type0 == VariantType::VAR_FLOAT &&
		type1 == VariantType::VAR_FLOAT
		)
	{
		product = Variant(factor0.GetFloat() * factor1.GetFloat());
	}
	else if (
		type0 == VariantType::VAR_FLOAT &&
		type1 == VariantType::VAR_VECTOR3
		)
	{
		product = Variant(factor0.GetFloat() * factor1.GetVector3());
	}
	else if (
		type0 == VariantType::VAR_VECTOR3 &&
		type1 == VariantType::VAR_FLOAT
		)
	{
		product = Variant(factor1.GetFloat() * factor0.GetVector3());
	}
	else if (
		type0 == VariantType::VAR_MATRIX3X4 &&
		type1 == VariantType::VAR_MATRIX3X4
		)
	{
		product = Variant(factor1.GetMatrix3x4() * factor0.GetMatrix3x4());
	}
	else if (
		type0 == VariantType::VAR_INT &&
		type1 == VariantType::VAR_INT
		)
	{
		product = Variant(factor0.GetInt() * factor1.GetInt());
	}
	else if (
		type0 == VariantType::VAR_INT &&
		type1 == VariantType::VAR_FLOAT
		)
	{
		product = Variant(factor0.GetInt() * factor1.GetFloat());
	}
	else if (
		type0 == VariantType::VAR_FLOAT &&
		type1 == VariantType::VAR_INT
		)
	{
		product = Variant(factor0.GetFloat() * factor1.GetInt());
	}
	else if (
		type0 == VariantType::VAR_QUATERNION &&
		type1 == VariantType::VAR_QUATERNION
		)
	{
		product = Variant(factor0.GetQuaternion() * factor1.GetQuaternion());
	}
	else {
		product = Variant();
	}

	outSolveInstance[0] = product;
}
