#include "Sets_Series.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Sets_Series::iconTexture = "Textures/Icons/Sets_Series.png";

Sets_Series::Sets_Series(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("Series");
	SetFullName("");
	SetDescription("Create a series of numbers");
	SetGroup(IoComponentGroup::SETS);
	SetSubgroup("Sequence");

	inputSlots_[0]->SetName("Start");
	inputSlots_[0]->SetVariableName("S");
	inputSlots_[0]->SetDescription("First number in the series");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDefaultValue(Variant(0.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Step");
	inputSlots_[1]->SetVariableName("N");
	inputSlots_[1]->SetDescription("Step size for each successive number");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDefaultValue(Variant(1.0f));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Count");
	inputSlots_[2]->SetVariableName("C");
	inputSlots_[2]->SetDescription("Number of values in the series");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDefaultValue(Variant(10));
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Series");
	outputSlots_[0]->SetVariableName("S");
	outputSlots_[0]->SetDescription("Series of numbers");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);
}

void Sets_Series::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////
	float S = inSolveInstance[0].GetFloat();
	float N = inSolveInstance[1].GetFloat();
	int C = inSolveInstance[2].GetInt();

	VariantVector seriesList;
	seriesList.Push(S);
	for (int i = 1; i < C; ++i) {
		seriesList.Push(S + i * N);
	}
	Variant out(seriesList);
	outSolveInstance[0] = out;

	////////////////////////////////////////////////////////////
}
