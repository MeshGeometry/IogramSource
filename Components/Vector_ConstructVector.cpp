#include "Vector_ConstructVector.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Vector_ConstructVector::iconTexture = "Textures/Icons/Vector_ConstructVector.png";

/*
WARNING: totally ripping off GH's Series node, trying to match the behavior.
*/
Vector_ConstructVector::Vector_ConstructVector(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("ConstructVector");
	SetFullName("Construct Vector");
	SetDescription("Construct a vector from xyz-coordinates");
	SetGroup(IoComponentGroup::VECTOR);
	SetSubgroup("Vector");

	inputSlots_[0]->SetName("X");
	inputSlots_[0]->SetVariableName("X");
	inputSlots_[0]->SetDescription("X coordinate");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDefaultValue(Variant(0.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Y");
	inputSlots_[1]->SetVariableName("Y");
	inputSlots_[1]->SetDescription("Y coordinate");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDefaultValue(Variant(0.0f));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Z");
	inputSlots_[2]->SetVariableName("Z");
	inputSlots_[2]->SetDescription("Z coordinate");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDefaultValue(Variant(0.0f));
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Vector3");
	outputSlots_[0]->SetVariableName("V");
	outputSlots_[0]->SetDescription("Vector3 out");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Vector_ConstructVector::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	/////////
	// VERIFY

	if (!(inSolveInstance[0].GetType() == VariantType::VAR_FLOAT ||
		inSolveInstance[0].GetType() == VariantType::VAR_INT)) {
		URHO3D_LOGWARNING("X must be a number");
		outSolveInstance[0] = Variant();
		return;
	}
	if (!(inSolveInstance[1].GetType() == VariantType::VAR_FLOAT ||
		inSolveInstance[1].GetType() == VariantType::VAR_INT)) {
		URHO3D_LOGWARNING("Y must be a number");
		outSolveInstance[0] = Variant();
		return;
	}
	if (!(inSolveInstance[2].GetType() == VariantType::VAR_FLOAT ||
		inSolveInstance[2].GetType() == VariantType::VAR_INT)) {
		URHO3D_LOGWARNING("Z must be a number");
		outSolveInstance[0] = Variant();
		return;
	}

	//////////
	// EXTRACT

	float x = inSolveInstance[0].GetFloat();
	float y = inSolveInstance[1].GetFloat();
	float z = inSolveInstance[2].GetFloat();

	///////////////////
	// COMPONENT'S WORK

	outSolveInstance[0] = Vector3(x,y,z);
}