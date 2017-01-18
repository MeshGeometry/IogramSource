#include "Tree_GetItem.h"

#include <assert.h>

using namespace Urho3D;

String Tree_GetItem::iconTexture = "Textures/Icons/Tree_GetItem.png";

Tree_GetItem::Tree_GetItem(Context* context) :
	IoComponentBase(context, 3, 1)
{
	SetName("GetTreeItem");
	SetFullName("Get Tree Item");
	SetDescription("Get Tree Item by path and index");
	SetGroup(IoComponentGroup::TREE);
	SetSubgroup("Operations");

	inputSlots_[0]->SetName("IoDataTree");
	inputSlots_[0]->SetVariableName("T");
	inputSlots_[0]->SetDescription("IoDataTree storing lookup item");
	inputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Path");
	inputSlots_[1]->SetVariableName("P");
	inputSlots_[1]->SetDescription("Path in tree to item");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDataAccess(DataAccess::LIST);
	inputSlots_[1]->SetDefaultValue(Urho3D::Variant(0));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Index");
	inputSlots_[2]->SetVariableName("I");
	inputSlots_[2]->SetDescription("Index of item in list");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(Urho3D::Variant(0));
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Item");
	outputSlots_[0]->SetVariableName("I");
	outputSlots_[0]->SetDescription("Item from tree");
	outputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

// Always attempts to solve the node anew, based on current input values, regardless of value of solvedFlag_
// Returns:
//   1 if the node is successfully solved
//   0 otherwise
int Tree_GetItem::LocalSolve()
{
	// check for empty IoDataTree values at IoInputSlots
	for (unsigned i = 0; i < inputSlots_.Size(); ++i) {
		if (inputSlots_[i]->HasNoData()) {

			solvedFlag_ = 0;
			return 0;
		}
	}

	// all IoInputSlots have non-empty IoDataTree values; proceed with LocalSolve operation
	lookupTree = SharedPtr<IoDataTree>(new IoDataTree(inputSlots_[0]->GetIoDataTree()));

	// Skip the first input slot!
	Vector<SharedPtr<IoDataTree> > inputIoDataTrees;
	for (unsigned i = 1; i < inputSlots_.Size(); ++i) {
		SharedPtr<IoDataTree> treePtr = SharedPtr<IoDataTree>(new IoDataTree(inputSlots_[i]->GetIoDataTree()));
		inputIoDataTrees.Push(treePtr);
	}

	Vector<SharedPtr<IoDataTree> > outputIoDataTrees;
	for (unsigned i = 0; i < outputSlots_.Size(); ++i) {
		SharedPtr<IoDataTree> treePtr = SharedPtr<IoDataTree>(new IoDataTree(GetContext()));
		outputIoDataTrees.Push(treePtr);
	}

	// Find branch count of the IoDataTree with the highest branch count
	// Skip the first input slot!
	unsigned maxNumBranches = inputIoDataTrees[0]->GetNumBranches();
	unsigned maxBranchIndex = 0;
	for (unsigned i = 1; i < inputIoDataTrees.Size(); ++i) {
		unsigned numBranches = inputIoDataTrees[i]->GetNumBranches();
		if (numBranches > maxNumBranches) {
			maxNumBranches = numBranches;
			maxBranchIndex = i;
		}
	}

	// currentPath[0] stores current path into IoDataTree at inputIoDataTrees index 0,
	// currentPath[1] stores current path into IoDataTree at inputIoDataTrees index 1,
	// and so on
	Vector<Vector<int> > currentPaths;
	for (unsigned i = 0; i < inputIoDataTrees.Size(); ++i) {
		currentPaths.Push(inputIoDataTrees[i]->Begin());
	}

	// loop one time for every branch in the highest branch count IoDataTree
	for (unsigned i = 0; i < maxNumBranches; ++i) {

		// A single "Arg" here refers to either a:
		//   Variant, if access is ITEM type;
		//   Vector<Variant>, if access is LIST type.
		// We loop over the input slots,
		// to find the maximum number of "Args" available from the lists identified by the currentPaths
		// WARNING: note index shift of +1 on input slot data access!
		unsigned maxNumArgs = inputIoDataTrees[0]->GetNumItemsAtBranch(currentPaths[0], inputSlots_[1]->GetDataAccess());
		for (unsigned j = 1; j < inputIoDataTrees.Size(); ++j) {
			unsigned numArgs = inputIoDataTrees[j]->GetNumItemsAtBranch(currentPaths[j], inputSlots_[j + 1]->GetDataAccess());
			if (numArgs > maxNumArgs) {
				maxNumArgs = numArgs;
			}
		}

		Vector<int> outputPath = inputIoDataTrees[maxBranchIndex]->GetCurrentBranch();

		// loop one time for every "Arg" available from the highest arg count
		for (unsigned j = 0; j < maxNumArgs; ++j) {

			Vector<Variant> inSolveInstance;
			for (unsigned k = 0; k < inputIoDataTrees.Size(); ++k) {
				Variant arg;
				inputIoDataTrees[k]->GetNextItem(arg, inputSlots_[k + 1]->GetDataAccess());
				inSolveInstance.Push(arg);
			}
			Vector<Variant> outSolveInstance(outputSlots_.Size());
			SolveInstance(inSolveInstance, outSolveInstance);

			for (unsigned k = 0; k < outputSlots_.Size(); ++k) {
				outputIoDataTrees[k]->Add(outputPath, outSolveInstance[k]);
			}
		}

		// update the identifiers in currentPaths
		currentPaths.Clear();
		for (unsigned j = 0; j < inputIoDataTrees.Size(); ++j) {
			currentPaths.Push(inputIoDataTrees[j]->GetNextBranch());
		}
	}

	for (unsigned i = 0; i < outputSlots_.Size(); ++i) {
		if (outputSlots_[i]->GetDataAccess() == DataAccess::LIST) {
			//std::cout << "here?" << std::endl;

			IoDataTree graftedTree = outputIoDataTrees[i]->OneToManyGraft();
			//std::cout << "graftedTree" << std::endl;
			//std::cout << graftedTree.ToString().CString() << std::endl;
			SharedPtr<IoDataTree> treePtr(new IoDataTree(graftedTree));
			outputIoDataTrees[i] = treePtr;
		}
	}

	for (unsigned i = 0; i < outputSlots_.Size(); ++i) {
		outputSlots_[i]->SetIoDataTree(*outputIoDataTrees[i]);
	}

	return solvedFlag_ = 1; // the only way that solvedFlag_ ever becomes 1
}

void Tree_GetItem::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() >= 2);

	Variant pathVariant = inSolveInstance[0];
	if (pathVariant.GetType() != VariantType::VAR_VARIANTVECTOR) {
		URHO3D_LOGERROR("ERROR: Tree_GetItem::SolveInstance --- bad path");
		outSolveInstance[0] = Variant();
		return;
	}
	
	Vector<int> path;
	VariantVector pathVariantVector = pathVariant.GetVariantVector();
	for (unsigned i = 0; i < pathVariantVector.Size(); ++i) {
		path.Push(pathVariantVector[i].GetInt());
	}

	int index = inSolveInstance[1].GetInt();

	Variant item;
	lookupTree->GetItem(item, path, index);

	assert(outSolveInstance.Size() == 1);
	outSolveInstance[0] = item;
}