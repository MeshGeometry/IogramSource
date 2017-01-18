#include "Maths_DotProduct.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_DotProduct::iconTexture = "Textures/Icons/Maths_DotProduct.png";

Maths_DotProduct::Maths_DotProduct(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("DotProduct");
	SetFullName("Dot Product");
	SetDescription("Computes the dot product of two vectors");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	Variant default_vec = Vector3(0.0f, 0.0f, 0.0f);

	inputSlots_[0]->SetName("firstVector");
	inputSlots_[0]->SetVariableName("X");
	inputSlots_[0]->SetDescription("First vector");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDefaultValue(default_vec);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("secondVector");
	inputSlots_[1]->SetVariableName("Y");
	inputSlots_[1]->SetDescription("Second vector");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[1]->SetDefaultValue(default_vec);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Dot Product");
	outputSlots_[0]->SetVariableName("D");
	outputSlots_[0]->SetDescription("Float representing X dot Y");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

}

void Maths_DotProduct::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	if (inSolveInstance[0].GetType() != VariantType::VAR_VECTOR3) {
		URHO3D_LOGWARNING("X must be a valid Vector3.");
		outSolveInstance[0] = Variant();
		return;
	}
	if (inSolveInstance[1].GetType() != VariantType::VAR_VECTOR3) {
		URHO3D_LOGWARNING("Y must be a valid Vector3.");
		outSolveInstance[0] = Variant();
		return;
	}

	Vector3 X = inSolveInstance[0].GetVector3();
	Vector3 Y = inSolveInstance[1].GetVector3();

	///////////////////
	// COMPONENT'S WORK

	Variant dot_prod;

	dot_prod = X.DotProduct(Y);

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = dot_prod;
}
