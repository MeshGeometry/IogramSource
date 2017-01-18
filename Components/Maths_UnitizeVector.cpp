#include "Maths_UnitizeVector.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_UnitizeVector::iconTexture = "Textures/Icons/Maths_UnitizeVector.png";

Maths_UnitizeVector::Maths_UnitizeVector(Urho3D::Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("UnitizeVector");
	SetFullName("UnitizeVector");
	SetDescription("Computes a unit vector from a vector");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	Variant default_vec = Vector3(0.0f, 0.0f, 0.0f);

	inputSlots_[0]->SetName("Vector");
	inputSlots_[0]->SetVariableName("X");
	inputSlots_[0]->SetDescription("Vector");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDefaultValue(default_vec);
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("UnitVector");
	outputSlots_[0]->SetVariableName("U");
	outputSlots_[0]->SetDescription("Unit Vector in direction of X");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

}

void Maths_UnitizeVector::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////
	Vector3 X = inSolveInstance[0].GetVector3();
	Vector3 default_vec = Vector3(0.0f, 0.0f, 0.0f);
	Variant unitVec;

	if (X == default_vec) {
		unitVec = default_vec;
	}
	else
		unitVec = X.Normalized();

	outSolveInstance[0] = unitVec;

	////////////////////////////////////////////////////////////
}
