#include "Maths_MassAverage.h"

#include <assert.h>

#include <iostream>

using namespace Urho3D;

String Maths_MassAverage::iconTexture = "Textures/Icons/Maths_MassAverage.png";

Maths_MassAverage::Maths_MassAverage(Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("MassAvg");
	SetFullName("Mass Average");
	SetDescription("Average of list of numbers");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("List");
	inputSlots_[0]->SetVariableName("L");
	inputSlots_[0]->SetDescription("List of floating point values");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);
	inputSlots_[0]->SetDefaultValue(Variant(0.0f));
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("Average");
	outputSlots_[0]->SetVariableName("A");
	outputSlots_[0]->SetDescription("Average of values");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Maths_MassAverage::SolveInstance(
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
	if (argsToAverage.Size() > 0) {
		result = result / argsToAverage.Size();
	}
	/////////////////////////////////////////////////////////

	outSolveInstance[0] = Variant(result);
}
