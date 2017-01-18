#include "Input_ColorWheel.h"

using namespace Urho3D;

String Input_ColorWheel::iconTexture = "Textures/Icons/Input_ColorWheel.png";

Input_ColorWheel::Input_ColorWheel(Urho3D::Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("ColorWheel");
	SetFullName("Color Wheel");
	SetDescription("...");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("INPUT");

	inputSlots_[0]->SetName("Intensity");
	inputSlots_[0]->SetVariableName("");
	inputSlots_[0]->SetDescription("Intensity of Bloom");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(75.0f);
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("Color");
	outputSlots_[0]->SetVariableName("C");
	outputSlots_[0]->SetDescription("Color");
	outputSlots_[0]->SetVariantType(VariantType::VAR_COLOR);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Input_ColorWheel::PreLocalSolve()
{

}

void Input_ColorWheel::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	outSolveInstance[0] = Color::RED;
}