#include "Tree_Reverse.h"

#include <assert.h>

using namespace Urho3D;

String Tree_Reverse::iconTexture = "Textures/Icons/Tree_Reverse.png";

Tree_Reverse::Tree_Reverse(Context* context) :
	IoComponentBase(context, 1, 1)
{
	SetName("Reverse");
	SetFullName("Reverse Tree");
	SetDescription("Perform reversal on a tree");
	SetGroup(IoComponentGroup::TREE);
	SetSubgroup("Operations");

	inputSlots_[0]->SetName("IoDataTree");
	inputSlots_[0]->SetVariableName("T");
	inputSlots_[0]->SetDescription("IoDataTree to reverse");
	inputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("IoDataTree");
	outputSlots_[0]->SetVariableName("S");
	outputSlots_[0]->SetDescription("Reversed IoDataTree");
	outputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}


int Tree_Reverse::LocalSolve()
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