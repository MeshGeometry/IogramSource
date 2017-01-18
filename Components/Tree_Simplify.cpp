#include "Tree_Simplify.h"

#include <assert.h>

using namespace Urho3D;

String Tree_Simplify::iconTexture = "Textures/Icons/Tree_Simplify.png";

Tree_Simplify::Tree_Simplify(Context* context) :
	IoComponentBase(context, 1, 1)
{
	SetName("Simplify");
	SetFullName("Simplify Tree");
	SetDescription("Perform simplification on a tree");
	SetGroup(IoComponentGroup::TREE);
	SetSubgroup("Operations");

	inputSlots_[0]->SetName("IoDataTree");
	inputSlots_[0]->SetVariableName("T");
	inputSlots_[0]->SetDescription("IoDataTree to simplify");
	inputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("IoDataTree");
	outputSlots_[0]->SetVariableName("S");
	outputSlots_[0]->SetDescription("Simplified IoDataTree");
	outputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}


int Tree_Simplify::LocalSolve()
{
	assert(inputSlots_.Size() >= 1);
	assert(outputSlots_.Size() == 1);

	if (inputSlots_[0]->HasNoData()) {
		solvedFlag_ = 0;
		return 0;
	}

	SharedPtr<IoDataTree> inputIoDataTree = SharedPtr<IoDataTree>(new IoDataTree(inputSlots_[0]->GetIoDataTree()));

	outputSlots_[0]->SetIoDataTree(inputIoDataTree->Simplify());

	solvedFlag_ = 1;
	return 1;
}