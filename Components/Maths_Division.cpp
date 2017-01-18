#include "Maths_Division.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_Division::iconTexture = "Textures/Icons/Maths_Division.png";

Maths_Division::Maths_Division(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("X/Y");
	SetFullName("Division");
	SetDescription("Mathematical division");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("X");
	inputSlots_[0]->SetVariableName("X");
	inputSlots_[0]->SetDescription("Number to divide");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant(1.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Y");
	inputSlots_[1]->SetVariableName("Y");
	inputSlots_[1]->SetDescription("Number to divide by");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(1.0f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Result");
	outputSlots_[0]->SetVariableName("R");
	outputSlots_[0]->SetDescription("Result of the division");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Maths_Division::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	Variant factor0 = inSolveInstance[0];
	Variant factor1 = inSolveInstance[1];

	VariantType type0 = factor0.GetType();
	VariantType type1 = factor1.GetType();

	Variant product;

	///////////////////
	// COMPONENT'S WORK

	if (
		type0 == VariantType::VAR_FLOAT &&
		type1 == VariantType::VAR_FLOAT
		)
	{
		// preserving original function logic in here
		float result = 1.0;
		if (inSolveInstance[1].GetFloat() == 0)
			result = M_INFINITY;
		else {
			result = inSolveInstance[0].GetFloat() / inSolveInstance[1].GetFloat();
		}
		product = Variant(result);
	}
	else if (
		type0 == VariantType::VAR_FLOAT &&
		type1 == VariantType::VAR_VECTOR3
		)
	{
		if (factor0.GetFloat() != 0.0f) {
			float scalar = 1.0f / factor0.GetFloat();
			product = Variant(scalar * factor1.GetVector3());
		}
		else {
			product = Variant();
		}
	}
	else if (
		type0 == VariantType::VAR_VECTOR3 &&
		type1 == VariantType::VAR_FLOAT
		)
	{
		if (factor1.GetFloat() != 0.0f) {
			float scalar = 1.0f / factor1.GetFloat();
			product = Variant(scalar * factor0.GetVector3());
		}
		else {
			product = Variant();
		}
	}
	else {
		product = Variant();
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = product;
}
