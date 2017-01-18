#include "Maths_LessThan.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_LessThan::iconTexture = "Textures/Icons/Maths_LessThan.png";

Maths_LessThan::Maths_LessThan(Urho3D::Context* context) : IoComponentBase(context, 2, 2)
{
	SetName("LessThan");
	SetFullName("Less Than");
	SetDescription("Tests if input B is less than input A");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("firstNumber");
	inputSlots_[0]->SetVariableName("A");
	inputSlots_[0]->SetDescription("First number");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDefaultValue(Variant(0.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("secondNumber");
	inputSlots_[1]->SetVariableName("B");
	inputSlots_[1]->SetDescription("Second number");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDefaultValue(Variant(0.0f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Less than (<)");
	outputSlots_[0]->SetVariableName("<");
	outputSlots_[0]->SetDescription("True if A < B");
	outputSlots_[0]->SetVariantType(VariantType::VAR_BOOL);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Less than or equal to (<=)");
	outputSlots_[1]->SetVariableName("<=");
	outputSlots_[1]->SetDescription("True if A <= B");
	outputSlots_[1]->SetVariantType(VariantType::VAR_BOOL);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Maths_LessThan::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////

	if (inSolveInstance[0].GetType() == VAR_NONE ||
		inSolveInstance[1].GetType() == VAR_NONE)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Variant lhs = inSolveInstance[0];
	Variant rhs = inSolveInstance[1];
	VariantType lhsType = lhs.GetType();
	VariantType rhsType = rhs.GetType();

	float A = inSolveInstance[0].GetFloat();
	float B = inSolveInstance[1].GetFloat();

	if (
		lhsType == VariantType::VAR_VECTOR3 &&
		rhsType == VariantType::VAR_VECTOR3
		)
	{
		A = lhs.GetVector3().Length();
		B = rhs.GetVector3().Length();
	}
	
	Variant less_than;
	Variant less_than_or_equal;

	if (A < B)
	{
		less_than = true;
		less_than_or_equal = true;
	}
	else if (A == B)
	{
		less_than = false;
		less_than_or_equal = true;
	}
	else
	{
		less_than = false;
		less_than_or_equal = false;
	}

	outSolveInstance[0] = less_than;
	outSolveInstance[1] = less_than_or_equal;

	////////////////////////////////////////////////////////////
}
