#include "Vector_ColorRGBA.h"

#include "Urho3D/Graphics/Technique.h"

using namespace Urho3D;

String Vector_ColorRGBA::iconTexture = "Textures/Icons/Vector_ColorRGBA.png";

Vector_ColorRGBA::Vector_ColorRGBA(Urho3D::Context* context) : IoComponentBase(context, 4, 1)
{
	SetName("Color");
	SetFullName("Construct Color");
	SetDescription("Construct a color from RGBA values");
	SetGroup(IoComponentGroup::VECTOR);
	SetSubgroup("Vector");

	inputSlots_[0]->SetName("R");
	inputSlots_[0]->SetVariableName("R");
	inputSlots_[0]->SetDescription("Red");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(0.1f);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("G");
	inputSlots_[1]->SetVariableName("G");
	inputSlots_[1]->SetDescription("Green");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(0.1f);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("B");
	inputSlots_[2]->SetVariableName("B");
	inputSlots_[2]->SetDescription("Blue");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(0.1f);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("A");
	inputSlots_[3]->SetVariableName("A");
	inputSlots_[3]->SetDescription("Alpha");
	inputSlots_[3]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(1.0f);
	inputSlots_[3]->DefaultSet();

	outputSlots_[0]->SetName("Color");
	outputSlots_[0]->SetVariableName("C");
	outputSlots_[0]->SetDescription("Color");
	outputSlots_[0]->SetVariantType(VariantType::VAR_COLOR);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}


void Vector_ColorRGBA::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{

	float r = Clamp(inSolveInstance[0].GetFloat(), 0.0f, 1.0f);
	float g = Clamp(inSolveInstance[1].GetFloat(), 0.0f, 1.0f);
	float b = Clamp(inSolveInstance[2].GetFloat(), 0.0f, 1.0f);
	float a = Clamp(inSolveInstance[3].GetFloat(), 0.0f, 1.0f);

	outSolveInstance[0] = Color(r, g, b, a);
}