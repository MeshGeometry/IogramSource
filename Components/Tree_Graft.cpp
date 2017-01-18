#include "Tree_Graft.h"

#include <assert.h>

using namespace Urho3D;

String Tree_Graft::iconTexture = "Textures/Icons/Tree_Graft.png";

Tree_Graft::Tree_Graft(Context* context) :
	IoComponentBase(context, 1, 1)
{
	SetName("Graft");
	SetFullName("Graft Tree");
	SetDescription("Perform grafting on a tree");
	SetGroup(IoComponentGroup::TREE);
	SetSubgroup("Operations");

	inputSlots_[0]->SetName("IoDataTree");
	inputSlots_[0]->SetVariableName("T");
	inputSlots_[0]->SetDescription("IoDataTree to graft");
	inputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("IoDataTree");
	outputSlots_[0]->SetVariableName("G");
	outputSlots_[0]->SetDescription("Grafted IoDataTree");
	outputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}


int Tree_Graft::LocalSolve()
{
	assert(inputSlots_.Size() >= 1);
	assert(outputSlots_.Size() == 1);

	if (inputSlots_[0]->HasNoData()) {
		solvedFlag_ = 0;
		return 0;
	}

	SharedPtr<IoDataTree> inputIoDataTree = SharedPtr<IoDataTree>(new IoDataTree(inputSlots_[0]->GetIoDataTree()));

	outputSlots_[0]->SetIoDataTree(inputIoDataTree->Graft());

	solvedFlag_ = 1;
	return 1;
}
