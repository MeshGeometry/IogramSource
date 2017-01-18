#include "Maths_VectorLength.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_VectorLength::iconTexture = "Textures/Icons/Maths_VectorLength.png";

Maths_VectorLength::Maths_VectorLength(Urho3D::Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("VectorLength");
	SetFullName("Vector Length");
	SetDescription("Computes the length of a vector");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	Variant default_vec = Vector3(0.0f, 0.0f, 0.0f);

	inputSlots_[0]->SetName("Vector");
	inputSlots_[0]->SetVariableName("X");
	inputSlots_[0]->SetDescription("Vector");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDefaultValue(default_vec);
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("Length");
	outputSlots_[0]->SetVariableName("L");
	outputSlots_[0]->SetDescription("Float representing length of X");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

}

void Maths_VectorLength::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	////////////////////////////////////////////////////////////
	if (inSolveInstance[0].GetType() != VariantType::VAR_VECTOR3) {
		outSolveInstance[0] = Variant();
		return;
	}

	Vector3 X = inSolveInstance[0].GetVector3();

	Variant length;

	length = X.Length();

	outSolveInstance[0] = length;

	////////////////////////////////////////////////////////////
}
