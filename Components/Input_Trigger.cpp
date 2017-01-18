#include "Input_Trigger.h"

#include <assert.h>

using namespace Urho3D;

String Input_Trigger::iconTexture = "Textures/Icons/Input_Trigger.png";

Input_Trigger::Input_Trigger(Context* context) :
	IoComponentBase(context, 2, 1)
{
	SetName("Trigger");
	SetFullName("Trigger");
	SetDescription("Transmit IoDataTree at first input upon trigger at second input");
	SetGroup(IoComponentGroup::TREE);
	SetSubgroup("Operations");

	inputSlots_[0]->SetName("IoDataTree");
	inputSlots_[0]->SetVariableName("In");
	inputSlots_[0]->SetDescription("IoDataTree input to transmit on trigger");
	inputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Trigger");
	inputSlots_[1]->SetVariableName("Tr");
	inputSlots_[1]->SetDescription("Trigger -- listen for change in trigger input");
	inputSlots_[1]->SetVariantType(VariantType::VAR_NONE);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("IoDataTree");
	outputSlots_[0]->SetVariableName("In");
	outputSlots_[0]->SetDescription("IoDataTree transmitted on trigger");
	outputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}


int Input_Trigger::LocalSolve()
{
	assert(inputSlots_.Size() >= 1);
	assert(outputSlots_.Size() == 1);

	if (inputSlots_[0]->HasNoData()) {
		solvedFlag_ = 0;
		return 0;
	}

	SharedPtr<IoDataTree> inputIoDataTree = SharedPtr<IoDataTree>(new IoDataTree(inputSlots_[0]->GetIoDataTree()));

	outputSlots_[0]->SetIoDataTree(*inputIoDataTree);

	solvedFlag_ = 1;
	return 1;
}