#include "Maths_CrossProduct.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

using namespace Urho3D;

String Maths_CrossProduct::iconTexture = "Textures/Icons/Maths_CrossProduct.png";

Maths_CrossProduct::Maths_CrossProduct(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("CrossProduct");
	SetFullName("Cross Product");
	SetDescription("Computes the cross product of two vectors");
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

	outputSlots_[0]->SetName("Cross Product");
	outputSlots_[0]->SetVariableName("C");
	outputSlots_[0]->SetDescription("Float representing X cross Y");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

}

void Maths_CrossProduct::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	///////////////////
	// EXTRACT & VERIFY

	if (!IsAllInputValid(inSolveInstance)) {
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Vector3 X = inSolveInstance[0].GetVector3();
	Vector3 Y = inSolveInstance[1].GetVector3();

	///////////////////
	// COMPONENT'S WORK

	Variant cross_prod;

	cross_prod = X.CrossProduct(Y);

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = cross_prod;
}
