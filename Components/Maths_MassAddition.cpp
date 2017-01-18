#include "Maths_MassAddition.h"

#include <assert.h>

#include <iostream>

using namespace Urho3D;

String Maths_MassAddition::iconTexture = "Textures/Icons/DefaultIcon.png";

Maths_MassAddition::Maths_MassAddition(Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("MassAdd");
	SetFullName("Mass Addition");
	SetDescription("Sums a list of numbers");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("List");
	inputSlots_[0]->SetVariableName("L");
	inputSlots_[0]->SetDescription("List of floating point values");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);
	inputSlots_[0]->SetDefaultValue(Variant(0.0f));
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("Sum");
	outputSlots_[0]->SetVariableName("S");
	outputSlots_[0]->SetDescription("Sum of values");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Maths_MassAddition::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	Variant variant = inSolveInstance[0];
	VariantVector argsToAverage = variant.GetVariantVector();

	/////////////////////////////////////////////////////////
	float result = 0.0f;
	for (unsigned i = 0; i < argsToAverage.Size(); ++i) {
		result += argsToAverage[i].GetFloat();
	}

	/////////////////////////////////////////////////////////

	outSolveInstance[0] = Variant(result);
}
