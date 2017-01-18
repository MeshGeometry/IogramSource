#include "IoComponentBase.h"

#include <assert.h>
#include <iostream>
#include <vector>

#include <Urho3D/Core/Variant.h>

#include "IndexUtilities.h"
#include "IoDataTree.h"
#include "IoInputSlot.h"
#include "IoOutputSlot.h"
#include "NetworkUtilities.h"

using namespace Urho3D;

String IoComponentBase::iconTexture = "Textures/DefaultIcon.png";
String IoComponentBase::tags = "All";

IoComponentBase::IoComponentBase(Context* context, int numInputs, int numOutputs) :
	inputSlots_(0),
	outputSlots_(0),
	solvedFlag_(0),
	Object(context)
{
	// create the input slots
	for (int i = 0; i < numInputs; ++i) {
		Urho3D::SharedPtr<IoInputSlot> slotPtr(new IoInputSlot(GetContext(), Urho3D::SharedPtr<IoComponentBase>(this)));
		inputSlots_.Push(slotPtr);
	}
	assert((int)inputSlots_.Size() == numInputs);

	// create the output slots
	for (int i = 0; i < numOutputs; ++i) {
		Urho3D::SharedPtr<IoOutputSlot> slotPtr(new IoOutputSlot(GetContext(), Urho3D::SharedPtr<IoComponentBase>(this)));
		outputSlots_.Push(slotPtr);
	}
	assert((int)outputSlots_.Size() == numOutputs);

	//TODO: remove this code and implement in UI
	Urho3D::SetRandomSeed(GetSubsystem<Time>()->GetSystemTime());
	float idVal = Urho3D::Random();
	ID = StringHash(String(idVal)).ToString();
	float x = Urho3D::Random(100.0f, 500.0f);
	float y = Urho3D::Random(100.0f, 500.0f);
	viewData.pos = Vector2(x, y);
	viewData.size = Vector2(100, 130);

}

void IoComponentBase::InputHardSet(int inputIndex, IoDataTree ioDataTree)
{
	assert(IndexInRange(inputIndex, GetNumInputs()));

	inputSlots_[inputIndex]->HardSet(ioDataTree);

	solvedFlag_ = 0;
}

///////////////
// connectivity

// this == "parent"
void IoComponentBase::ConnectChild(
	Urho3D::SharedPtr<IoComponentBase> child,
	int indexIntoChildInput,
	int indexIntoParentOutput
	)
{
	if ((int)outputSlots_.Size() > indexIntoParentOutput &&
		(int)child->inputSlots_.Size() > indexIntoChildInput) {
		Urho3D::SharedPtr<IoOutputSlot> out = outputSlots_[indexIntoParentOutput];
		Urho3D::SharedPtr<IoInputSlot> in = child->inputSlots_[indexIntoChildInput];
		::mConnect(out, in);
	}
}

// this == "parent"
void IoComponentBase::DisconnectChild(int indexIntoParentOutput)
{
	Urho3D::SharedPtr<IoOutputSlot> out = outputSlots_[indexIntoParentOutput];

	::mDisconnect(out);
}

// this == "child"
void IoComponentBase::DisconnectParent(int indexIntoChildInput)
{
	Urho3D::SharedPtr<IoOutputSlot> out = inputSlots_[indexIntoChildInput]->GetLinkedOutputSlot();
	if (out.NotNull()) {

		Urho3D::SharedPtr<IoInputSlot> in = inputSlots_[indexIntoChildInput];
		::mDisconnect(in);
	}
}

// this == "parent"
void IoComponentBase::DisconnectAllChildren()
{
	for (int i = 0; i < (int)outputSlots_.Size(); ++i) {
		DisconnectChild(i);
	}
}

// this == "child"
void IoComponentBase::DisconnectAllParents()
{
	for (int i = 0; i < (int)inputSlots_.Size(); ++i) {
		DisconnectParent(i);
	}
}

int IoComponentBase::ComputeInDegree() const
{
	int inCount = 0;
	for (int i = 0; i < (int)inputSlots_.Size(); ++i) {
		if (inputSlots_[i]->GetLinkedOutputSlot().NotNull()) {
			++inCount;
		}
	}

	return inCount;
}

// Returns list of unique pointers to components immediately downstream of this component
Vector<SharedPtr<IoComponentBase> > IoComponentBase::GetUniqueComponentsOut() const
{
	Vector<SharedPtr<IoComponentBase> > downstreamComps;

	for (unsigned i = 0; i < outputSlots_.Size(); ++i) {
		Vector<SharedPtr<IoInputSlot> > inSlots = outputSlots_[i]->GetLinkedInputSlots();
		for (unsigned j = 0; j < inSlots.Size(); ++j) {
			if (inSlots[j].NotNull()) {
				downstreamComps.Push(inSlots[j]->GetHomeComponent());
			}
		}
	}

	Vector<SharedPtr<IoComponentBase> > uniqueDownstreamComps;

	// go through the components in downstreamComps and add to uniqueDownstreamComps every time we hit a new comp
	for (unsigned i = 0; i < downstreamComps.Size(); ++i) {
		SharedPtr<IoComponentBase> downComp = downstreamComps[i];

		// Push downComp onto uniqueDownstreamComps -- but only if it does not yet appear among the uniques
		if (uniqueDownstreamComps.Find(downComp) == uniqueDownstreamComps.End()) {
			uniqueDownstreamComps.Push(downComp);
		}
	}

	return uniqueDownstreamComps;
}

// Returns list of unique pointers to components immediately upstream of this component
Vector<SharedPtr<IoComponentBase> > IoComponentBase::GetUniqueComponentsIn() const
{
	Vector<SharedPtr<IoComponentBase> > upstreamComps;

	for (unsigned i = 0; i < inputSlots_.Size(); ++i) {
		SharedPtr<IoOutputSlot> linkedOut = inputSlots_[i]->GetLinkedOutputSlot();
		if (linkedOut.NotNull()) {
			upstreamComps.Push(linkedOut->GetHomeComponent());
		}
	}

	Vector<SharedPtr<IoComponentBase> > uniqueUpstreamComps;

	// go through the components in upstreamComps and add to uniqueUpstreamComps every time we hit a new comp
	for (unsigned i = 0; i < upstreamComps.Size(); ++i) {
		SharedPtr<IoComponentBase> upComp = upstreamComps[i];

		// Push upComp onto uniqueUpstreamComps -- but only if it does not yet appear among the uniques
		if (uniqueUpstreamComps.Find(upComp) == uniqueUpstreamComps.End()) {
			uniqueUpstreamComps.Push(upComp);
		}
	}

	return uniqueUpstreamComps;
}

// Returns the number of unique components immediately upstream of this component
unsigned IoComponentBase::ComputeUniqueInDegree() const
{
	return GetUniqueComponentsIn().Size();
}

// Returns the number of unique compenents immediately downstream of this component
unsigned IoComponentBase::ComputeUniqueOutDegree() const
{
	return GetUniqueComponentsOut().Size();
}

int IoComponentBase::ComputeOutDegree() const
{
	int outCount = 0;
	for (int i = 0; i < (int)outputSlots_.Size(); ++i) {
		outCount += outputSlots_[i]->GetNumLinkedInputSlots();
	}

	return outCount;
}

///////////////////////////
// computation at this node

void IoComponentBase::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	for (unsigned i = 0; i < outSolveInstance.Size(); ++i) {
		outSolveInstance[i] = Variant(0.0f);
	}
}

// Always attempts to solve the node anew, based on current input values, regardless of value of solvedFlag_
// Returns:
//   1 if the node is successfully solved
//   0 otherwise
int IoComponentBase::LocalSolve()
{
	// check for empty IoDataTree values at IoInputSlots
	for (unsigned i = 0; i < inputSlots_.Size(); ++i) {
		if (inputSlots_[i]->HasNoData()) {

			// shot in dark, C.C. 7/12/2016
			ClearOutputs();

			solvedFlag_ = 0;
			return 0;
		}
	}

	//call a prep function if needed
	PreLocalSolve();

	// all IoInputSlots have non-empty IoDataTree values; proceed with LocalSolve operation

	// Vector<SharedPtr<IoDataTree> > not Vector<IoDataTree> just b/c Vector<IoDataTree> can't compile b/c IoDataTree has no default constructor
	Vector<SharedPtr<IoDataTree> > inputIoDataTrees;
	for (unsigned i = 0; i < inputSlots_.Size(); ++i) {
		SharedPtr<IoDataTree> treePtr = SharedPtr<IoDataTree>(new IoDataTree(inputSlots_[i]->GetIoDataTree()));
		inputIoDataTrees.Push(treePtr);
	}

	Vector<SharedPtr<IoDataTree> > outputIoDataTrees;
	for (unsigned i = 0; i < outputSlots_.Size(); ++i) {
		SharedPtr<IoDataTree> treePtr = SharedPtr<IoDataTree>(new IoDataTree(GetContext()));
		outputIoDataTrees.Push(treePtr);
	}

	// find branch count of the IoDataTree with the highest branch count
	unsigned maxNumBranches = inputIoDataTrees[0]->GetNumBranches();
	unsigned maxBranchIndex = 0;
	for (unsigned i = 1; i < inputIoDataTrees.Size(); ++i) {
		unsigned numBranches = inputIoDataTrees[i]->GetNumBranches();
		if (numBranches > maxNumBranches) {
			maxNumBranches = numBranches;
			maxBranchIndex = i;
		}
	}

	// currentPath[0] stores current path into IoDataTree at IoInputSlot index 0,
	// currentPath[1] stores current path into IoDataTree at IoInputSlot index 1,
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
		unsigned maxNumArgs = inputIoDataTrees[0]->GetNumItemsAtBranch(currentPaths[0], inputSlots_[0]->GetDataAccess());
		for (unsigned j = 1; j < inputIoDataTrees.Size(); ++j) {
			unsigned numArgs = inputIoDataTrees[j]->GetNumItemsAtBranch(currentPaths[j], inputSlots_[j]->GetDataAccess());
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
				inputIoDataTrees[k]->GetNextItem(arg, inputSlots_[k]->GetDataAccess());
				inSolveInstance.Push(arg);
			}
			Vector<Variant> outSolveInstance(outputSlots_.Size());
			SolveInstance(inSolveInstance, outSolveInstance);

			for (unsigned k = 0; k < outputSlots_.Size(); ++k) {
				outputIoDataTrees[k]->Add(outputPath, outSolveInstance[k]);
				/*
				Worrying at this point about 1-to-many problems.
				Maybe it's cleanest & simplest to add a Variant here, whether it
				represents a VAR_FLOAT or a VAR_VARIANTVECTOR or whatever,
				and afterwards traverse the tree and detect and push the lists to new branches
				where necessary....
				*/
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
			//std::cout << "... outputIoDataTree[" << i << "]->OneToManyGraft()";

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

void IoComponentBase::ClearOutputs()
{
	for (unsigned i = 0; i < outputSlots_.Size(); ++i) {
		IoDataTree ioDataTree(GetContext());
		outputSlots_[i]->SetIoDataTree(ioDataTree);
	}

	VariantMap data;
	data["component"] = this;
	SendEvent("OutputsCleared", data);

	solvedFlag_ = 0;
}

void IoComponentBase::AddInputSlot()
{
	Urho3D::SharedPtr<IoInputSlot> xslotPtr(new IoInputSlot(GetContext(), Urho3D::SharedPtr<IoComponentBase>(this)));
	inputSlots_.Push(xslotPtr);

	solvedFlag_ = 0;
}

void IoComponentBase::AddOutputSlot()
{
	Urho3D::SharedPtr<IoOutputSlot> slotPtr(new IoOutputSlot(GetContext(), Urho3D::SharedPtr<IoComponentBase>(this)));
	outputSlots_.Push(slotPtr);

	solvedFlag_ = 0;
}

void IoComponentBase::AddInputSlot(
		String name,
		String variableName,
		String description,
		VariantType type,
		DataAccess dataAccess)
{
	Urho3D::SharedPtr<IoInputSlot> xslotPtr(new IoInputSlot(GetContext(), Urho3D::SharedPtr<IoComponentBase>(this)));
	inputSlots_.Push(xslotPtr);

	xslotPtr->SetName(name);
	xslotPtr->SetVariableName(variableName);
	xslotPtr->SetDescription(description);
	xslotPtr->SetVariantType(type);
	xslotPtr->SetDataAccess(dataAccess);

	if (dataAccess == DataAccess::LIST)
	{
		xslotPtr->SetVariantType(VAR_VARIANTVECTOR);
	}

	VariantMap data;
	data["inputSlotIndex"] = inputSlots_.Size() - 1;
	data["variableName"] = variableName;
	data["name"] = name;
	data["description"] = description;
	data["type"] = type;

	SendEvent("NewInputSlotAdded", data);

	solvedFlag_ = 0;
}

void IoComponentBase::AddInputSlot(
	String name,
	String variableName,
	String description,
	VariantType type,
	DataAccess dataAccess,
	Variant defaultValue)
{
	Urho3D::SharedPtr<IoInputSlot> xslotPtr(new IoInputSlot(GetContext(), Urho3D::SharedPtr<IoComponentBase>(this)));
	inputSlots_.Push(xslotPtr);

	xslotPtr->SetName(name);
	xslotPtr->SetVariableName(variableName);
	xslotPtr->SetDescription(description);
	xslotPtr->SetVariantType(type);
	xslotPtr->SetDataAccess(dataAccess);

	if (dataAccess == DataAccess::LIST)
	{
		xslotPtr->SetVariantType(VAR_VARIANTVECTOR);
		VariantVector defVal;
		defVal.Push(defaultValue);
		xslotPtr->SetDefaultValue(defaultValue);
		xslotPtr->DefaultSet();
	}
	else 
	{
		xslotPtr->SetDefaultValue(defaultValue);
		xslotPtr->DefaultSet();
	}


	VariantMap data;
	data["inputSlotIndex"] = inputSlots_.Size() - 1;
	data["variableName"] = variableName;
	data["name"] = name;
	data["description"] = description;
	data["type"] = type;

	SendEvent("NewInputSlotAdded", data);

	solvedFlag_ = 0;
}

void IoComponentBase::AddOutputSlot(
		String name,
		String variableName,
		String description,
		VariantType type,
		DataAccess dataAccess)
{
	Urho3D::SharedPtr<IoOutputSlot> xslotPtr(new IoOutputSlot(GetContext(), Urho3D::SharedPtr<IoComponentBase>(this)));
	outputSlots_.Push(xslotPtr);

	xslotPtr->SetName(name);
	xslotPtr->SetVariableName(variableName);
	xslotPtr->SetDescription(description);
	xslotPtr->SetVariantType(type);
	xslotPtr->SetDataAccess(dataAccess);

	VariantMap data;
	data["outputSlotIndex"] = outputSlots_.Size() - 1;
	data["variableName"] = variableName;
	data["name"] = name;
	data["description"] = description;
	data["type"] = type;
	
	SendEvent("NewOutputSlotAdded", data);

	solvedFlag_ = 0;
}

void IoComponentBase::DeleteInputSlot(int index)
{
	assert(GetNumInputs() > 1);
	assert(IndexInRange(index, GetNumInputs()));

	::mDisconnect(inputSlots_[index]);
	inputSlots_.Erase(index);
	solvedFlag_ = 0;
}

void IoComponentBase::DeleteOutputSlot(int index)
{
	assert(GetNumOutputs() > 1);
	assert(IndexInRange(index, GetNumOutputs()));

	::mDisconnect(outputSlots_[index]);

	outputSlots_.Erase(index);
	solvedFlag_ = 0;
}

int IoComponentBase::GetNumOutgoingLinks(int outputSlotIdx) const
{
	assert(outputSlotIdx < GetNumOutputs());
	int numOut = 0;
	numOut = outputSlots_[outputSlotIdx]->GetNumLinkedInputSlots();
	return numOut;
}

// not implemented yet
int IoComponentBase::GetNumIncomingLinks(int inputSlotIdx) const
{
	assert(inputSlotIdx < GetNumInputs());
	int numIn = 0;

	return numIn;
}


Pair<SharedPtr<IoComponentBase>, int> IoComponentBase::GetIncomingLink(int inputSlotIdx)
{
	assert(inputSlotIdx < GetNumInputs());

	Pair<SharedPtr<IoComponentBase>, int> pairOut;
	pairOut.first_ = NULL;
	pairOut.second_ = -1;
	SharedPtr<IoOutputSlot> outputSlot = inputSlots_[inputSlotIdx]->GetLinkedOutputSlot();
	if (outputSlot.NotNull())
	{
		SharedPtr<IoComponentBase> homeComp = outputSlot->GetHomeComponent();
		if (outputSlot.NotNull() && homeComp.NotNull())
		{
			pairOut.first_ = homeComp;
			pairOut.second_ = homeComp->outputSlots_.Find(outputSlot) - homeComp->outputSlots_.Begin();
		}
	}

	return  pairOut;
}

int IoComponentBase::GetInputSlotIndex(SharedPtr<IoInputSlot> inputSlot) const
{
	assert(inputSlot.NotNull());
	int index = inputSlots_.Find(inputSlot) - inputSlots_.Begin();
	return index;
}
int IoComponentBase::GetOutputSlotIndex(SharedPtr<IoOutputSlot> outputSlot) const
{
	assert(outputSlot.NotNull());
	int index = outputSlots_.Find(outputSlot) - outputSlots_.Begin();
	return index;
}

Pair<SharedPtr<IoComponentBase>, int> IoComponentBase::GetOutgoingLink(int outputSlotIdx, int linkIdx)
{
	assert(outputSlotIdx < GetNumOutputs());
	assert(linkIdx < (int)outputSlots_[outputSlotIdx]->GetNumLinkedInputSlots());

	Pair<SharedPtr<IoComponentBase>, int> pairOut;
	pairOut.first_ = NULL;
	pairOut.second_ = -1;
	SharedPtr<IoInputSlot> inputSlot = outputSlots_[outputSlotIdx].Get()->GetLinkedInputSlot(linkIdx);
	if (inputSlot.NotNull())
	{
		SharedPtr<IoComponentBase> homeComp = inputSlot->GetHomeComponent();
		if (inputSlot.NotNull() && homeComp.NotNull())
		{
			pairOut.first_ = homeComp;
			pairOut.second_ = homeComp->inputSlots_.Find(inputSlot) - homeComp->inputSlots_.Begin();
		}
	}

	return  pairOut;
}


IoDataTree IoComponentBase::GetOutputIoDataTree(unsigned index)
{
	assert(IndexInRange(index, GetNumOutputs()));

	return outputSlots_[index]->GetIoDataTree();
}

// WARNING 1: returns null pointers for output slots without linked input nodes
// WARNING 2: transition to Vector pending
std::vector<Urho3D::SharedPtr<IoComponentBase> > IoComponentBase::GetChildren()
{
	std::vector<Urho3D::SharedPtr<IoComponentBase> > children;

	for (int i = 0; i < (int)outputSlots_.Size(); ++i) {
		Urho3D::Vector<Urho3D::SharedPtr<IoInputSlot> > iSlots = outputSlots_[i]->GetLinkedInputSlots();
		if ((int)iSlots.Size() > 0) {
			for (int j = 0; j < (int)iSlots.Size(); ++j) {
				Urho3D::SharedPtr<IoInputSlot> in = iSlots[j];
				if (in.NotNull()) {
					children.push_back(in->GetHomeComponent());
				}
			}
		}
		else {
			children.push_back(Urho3D::SharedPtr<IoComponentBase>());
		}
	}

	return children;
}

String IoComponentBase::GetNodeStyle()
{
	return "BasicInputNode";
}

void IoComponentBase::HandleCustomInterface(UIElement* customElement)
{

}

Urho3D::String IoComponentBase::VariantTypeToString(Urho3D::VariantType variantType) const
{
	String ret = "";

	switch (variantType) {
	case (VariantType::VAR_BOOL):
		ret = "VAR_BOOL";
		break;
	case (VariantType::VAR_STRING):
		ret = "VAR_STRING";
		break;
	case (VariantType::VAR_FLOAT):
		ret = "VAR_FLOAT";
		break;
	case (VariantType::VAR_INT):
		ret = "VAR_INT";
		break;
	case (VariantType::VAR_MATRIX3X4):
		ret = "VAR_MATRIX3X4";
		break;
	case (VariantType::VAR_VECTOR3):
		ret = "VAR_VECTOR3";
		break;
	case (VariantType::VAR_QUATERNION):
		ret = "VAR_QUATERNION";
		break;
	default:
		break;
	}

	return ret;
}

bool IoComponentBase::IsInputValid(unsigned inputIndex, const Urho3D::Variant& inputValue) const
{
	VariantType expectedVarType = inputSlots_[inputIndex]->GetVariantType();
	if (expectedVarType == VariantType::VAR_NONE) {
		return true;
	}

	if (inputSlots_[inputIndex]->GetDataAccess() == DataAccess::LIST) {
		// can't easily deal with this case, just say the input is valid if it stores VariantVector
		if (inputValue.GetType() == VariantType::VAR_VARIANTVECTOR) {
			return true;
		}
		else {
			return false;
		}
	}

	if (expectedVarType != inputValue.GetType()) {
		String variableName = inputSlots_[inputIndex]->GetVariableName();
		String variantTypeName = VariantTypeToString(expectedVarType);
		URHO3D_LOGWARNING(variableName + " must be a Variant of type " + variantTypeName);
		return false;
	}

	return true;
}

bool IoComponentBase::IsAllInputValid(const Urho3D::Vector<Urho3D::Variant>& inSolveInstance) const
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	for (unsigned i = 0; i < inputSlots_.Size(); ++i) {
		if (!IsInputValid(i, inSolveInstance[i])) {
			return false;
		}
	}

	return true;
}

void IoComponentBase::SetAllOutputsNull(Urho3D::Vector<Urho3D::Variant>& outSolveInstance)
{
	for (unsigned i = 0; i < outSolveInstance.Size(); ++i) {
		outSolveInstance[i] = Variant();
	}
}

bool IoComponentBase::SetGenericData(String key, Variant data)
{
	if (data.GetType() == VAR_VARIANTVECTOR
		|| data.GetType() == VAR_VARIANTMAP
		|| data.GetType() == VAR_VOIDPTR
		|| data.GetType() == VAR_PTR)
	{
		URHO3D_LOGERROR("Persistent Data doesn't support this type: " + data.GetTypeName());
		return false;
	}

	Pair<String, Variant> newPair(key, data);
	metaData_[key] = newPair;

	return true;
}

Variant IoComponentBase::GetGenericData(String key)
{
	Pair<String, Variant> p = metaData_[key];
	return p.second_;
}