//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "IoDataTree.h"

#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/IO/Log.h>

#include <iostream>

#include <assert.h>

using namespace Urho3D;

IoDataTree::IoDataTree(Context* context, Urho3D::Variant item) :
	Object(context)
{
	Vector<int> path;
	path.Push(0);
	Add(path, item);
}

IoDataTree::IoDataTree(Context* context, Urho3D::Vector<Urho3D::Variant> items) :
	Object(context)
{
	Vector<int> path;
	path.Push(0);
	Add(path, items);
}

IoDataTree::~IoDataTree()
{
	HashMap<String, IoBranch*>::ConstIterator it;
	for (it = branches_.Begin(); it != branches_.End(); ++it) {
		delete it->second_;
	}
}

IoDataTree::IoDataTree(const IoDataTree& original) : IoDataTree(original.GetContext())
{
	HashMap<String, IoBranch*>::ConstIterator it;
	for (it = original.branches_.Begin(); it != original.branches_.End(); ++it) {
		String pathString = it->first_;
		Vector<int> path = PathFromUniqueString(pathString);

		Vector<Variant> data = it->second_->data;

		branches_[pathString] = new IoBranch(path);
		branches_[pathString]->data = data;
	}
}

IoDataTree& IoDataTree::operator=(const IoDataTree& rhs)
{
	if (this != &rhs) {
		HashMap<String, IoBranch*>::ConstIterator it;
		for (it = branches_.Begin(); it != branches_.End(); ++it) {
			delete it->second_;
		}

		branches_.Clear();
		lastItemIndex_ = 0; // ?
		branchOverflow_ = false;
		itemOverflow_ = false;

		HashMap<String, IoBranch*>::ConstIterator rhsIt;
		for (rhsIt = rhs.branches_.Begin(); rhsIt != rhs.branches_.End(); ++rhsIt) {
			String pathString = rhsIt->first_;
			Vector<int> path = PathFromUniqueString(pathString);

			Vector<Variant> data = rhsIt->second_->data;

			branches_[pathString] = new IoBranch(path);
			branches_[pathString]->data = data;
		}
	}
	return *this;
}

String IoDataTree::PathToUniqueString(Vector<int> path) const
{
	String out;
	for (unsigned i = 0; i < path.Size(); i++)
	{
		out += String(path[i]) + "_";
	}

	return out;
}

Vector<int> IoDataTree::PathFromUniqueString(String pathString) const
{
	Vector<int> pathOut;
	Vector<String> indices = pathString.Split('_', false);
	for(unsigned i = 0; i < indices.Size(); i++)
	{
		pathOut.Push(ToInt(indices[i].CString()));
	}

	return pathOut;

}

void IoDataTree::Add(Vector<int> path, Variant item)
{
	//check that this path exists
	String pathString = PathToUniqueString(path);
	IoBranch* branchId = branches_[pathString];
	if (branchId == NULL)
	{
		//branch doesn't exist, so create it and add it to the map
		IoBranch* newBranch = new IoBranch(path);
		branches_[pathString] = newBranch;
	}

	//add the data
	branches_[pathString]->data.Push(item);
	
	//reset iterators
	Begin();
}

void IoDataTree::GetItem(Variant& item, Vector<int> path, int index) const
{
	String pathString = PathToUniqueString(path);
	IoBranch** branchPtr = branches_[pathString];

	IoBranch* branch = NULL;

	if (branchPtr != NULL) {
		branch = *branchPtr;
	}

	if (branch != NULL)
	{
		if (index < (int)branch->data.Size())
		{
			item = branch->data[index];
		}
	}
}

void IoDataTree::Add(Vector<int> path, VariantVector list)
{
	//check that this path exists
	String pathString = PathToUniqueString(path);
	IoBranch* branchId = branches_[pathString];
	if (branchId == NULL)
	{
		//branch doesn't exist, so create it and add it to the map
		IoBranch* newBranch = new IoBranch(path);
		branches_[pathString] = newBranch;
	}

	//add the data
	for (unsigned i = 0; i < list.Size(); i++)
	{
		branches_[pathString]->data.Push(list[i]);
	}

	//reset iterators
	Begin();
}

unsigned IoDataTree::GetNumItemsAtBranch(Vector<int> path, DataAccess accessType) const
{
	/*
	IoBranch** branchPtr = branches_[PathToUniqueString(path)];
	IoBranch* branch = NULL;
	if (branchPtr != NULL) {
		branch = *branchPtr;
	}
	*/

	if (accessType == DataAccess::ITEM) {
		if (branches_[PathToUniqueString(path)])
			return (*(branches_[PathToUniqueString(path)]))->data.Size();
		else
			return 1;
		//return branch->data.Size();
	}
	else {
		return 1; // change to 0 if LocalSolve is ready to handle this
	}
}

// Assumption:
//   All Variants stored anywhere in tree have same VariantType, and in addition if they
//   are VAR_VARIANTMAPS storing a custom type (e.g., TriMesh) all Variants share that custom type too.
// This function copies one data item (no guarantee which one) onto dataOut.
// The caller can then investigate dataOut manually to extract type information.
// Finding type stored in an IoDataTree is proving to be error prone:
// this functions aims to aid that lookup, without changing state of the tree and
// without crashing. Hopefully!
void IoDataTree::LookupType(Variant& dataOut, DataAccess accessType) const
{
	HashMap<String, IoBranch*>::ConstIterator it;
	for (it = branches_.Begin(); it != branches_.End(); ++it) {
		if (it->second_) {
			IoBranch* currentBranch = it->second_;
			if (currentBranch == NULL) {
				dataOut = Variant();
				return;
			}
			else {
				VariantVector dat = currentBranch->data;
				if (dat.Size() > 0) {
					dataOut = dat[0];
					return;
				}
			}
		}
	}

	dataOut = Variant();
	return;
}

void IoDataTree::GetNextItem(Variant& dataOut, DataAccess accessType)
{
	IoBranch* currentBranch = branchIterator_->second_;
	if(currentBranch == NULL)
	{
		return;
	}

	if (accessType == DataAccess::ITEM)
	{
		if (currentBranch->data.Size() == 0) {
			dataOut = Variant();
			itemOverflow_ = true;
			lastItemIndex_ = 0;
			return;
		}

		assert((int)currentBranch->data.Size() > lastItemIndex_); //WRONG! Triggers error on empty VariantVector...
		
		//first try to continue returning the next item on current branch index
		dataOut = currentBranch->data[lastItemIndex_];
		lastItemIndex_++;

		if (lastItemIndex_ > (int)currentBranch->data.Size() - 1)
		{
			itemOverflow_ = true;
		}

		lastItemIndex_ = Urho3D::Min(currentBranch->data.Size() - 1, lastItemIndex_);
	}

	if (accessType == DataAccess::LIST)
	{
		dataOut = currentBranch->data;
		itemOverflow_ = true;
		lastItemIndex_ = 0;
	}

}

String IoDataTree::ToString(bool truncate) const
{
	/*
	if (IsEmptyTree()) {
		return String("");
	}
	*/

	HashMap<String, IoBranch*>::ConstIterator itr = branches_.Begin();
	String out;
	int branchCounter = 0;
	for (; itr != branches_.End(); itr++)
	{
		if (truncate && branchCounter > 5)
			return out;

		String path = itr->first_;
		String itemCount = String(itr->second_->data.Size());
		out += "Branch: " + path + ", N = " + itemCount + "\n";
		int numItems = itr->second_->data.Size();
		if (truncate)
			numItems = Min(numItems, 3);

		for(int i = 0; i < numItems; i++)
		{
			Variant var = itr->second_->data[i];
            String type = var.GetTypeName();
            if (var.GetType() == VAR_VARIANTMAP){
                VariantMap var_map = var.GetVariantMap();
                if (var_map.Contains("type"))
                    type = var_map["type"].GetString();
            }
                
			out += "    " + var.ToString() + ", type: " + type + "\n";
		}

		if (truncate && itr->second_->data.Size() > 3)
		{
			out += "....and so on\n";
		}

		branchCounter++;
	}

	return out;
}

Urho3D::VariantMap IoDataTree::ToVariantMap() const
{
	HashMap<String, IoBranch*>::ConstIterator itr = branches_.Begin();
	VariantMap vm;

	for (; itr != branches_.End(); itr++) {

		String path = itr->first_;
		VariantVector data = itr->second_->data;
		vm[path.CString()] = Variant(data);
	}

	return vm;
}

Urho3D::Vector<Urho3D::String> IoDataTree::GetContent()
{
	HashMap<String, IoBranch*>::ConstIterator itr = branches_.Begin();
	Vector<String> contents;
	int branchCounter = 0;
	for (; itr != branches_.End(); itr++)
	{		
		String path = itr->first_;
		String itemCount = String(itr->second_->data.Size());
		int numItems = itr->second_->data.Size();

		for (int i = 0; i < numItems; i++)
		{
			Variant var = itr->second_->data[i];
			if (var.GetType() == VAR_NONE)
			{

			}
			else
			{
				String strVal = var.ToString();
				contents.Push(strVal);
			}

		}

		branchCounter++;
	}

	return contents;
}

// Returns a list of the immediate descendant branches growing out of the path stored in path.
// These are branches that are already present in the tree.
Vector<Vector<int> > IoDataTree::FindChildPaths(Vector<int> path) const
{
	Vector<Vector<int> > childPaths;

	HashMap<String, IoBranch*>::ConstIterator it;

	for (it = branches_.Begin(); it != branches_.End(); ++it) {
		String curPathString = it->first_;
		Vector<int> curPath = PathFromUniqueString(curPathString);

		if (IsParentChildPathPair(path, curPath)) {
			childPaths.Push(curPath);
		}
	}

	return childPaths;
}

// Computes the address for a new branch growing out of path.
Vector<int> IoDataTree::GetNextNewBranchPath(Vector<int> path) const
{
	Vector<int> copyPath = path;

	Vector<Vector<int> > childPaths = FindChildPaths(path);

	if (childPaths.Size() == 0) {
		copyPath.Push(0);
		return copyPath;
	}

	//Sort(childPaths.Begin(), childPaths.End(), ComparePaths);
	Sort(childPaths.Begin(), childPaths.End(), ComparePaths);
	Vector<int> lastBranchFullPath = childPaths[childPaths.Size() - 1];
	int lastBranchLastInt = lastBranchFullPath[lastBranchFullPath.Size() - 1];
	copyPath.Push(lastBranchLastInt + 1);
	return copyPath;
}

bool IoDataTree::HasSiblings(Vector<int> path) const
{
	HashMap<String, IoBranch*>::ConstIterator it;

	for (it = branches_.Begin(); it != branches_.End(); ++it) {
		String curPathString = it->first_;
		Vector<int> curPath = PathFromUniqueString(curPathString);

		if (WitnessSiblings(path, curPath)) {
			// curPath witnesses existence of a sibling for path
			return true;
		}
	}

	// no path in the tree has witnessed any sibling for path
	return false;
}

bool IoDataTree::CanBeDeleted(Vector<int> path) const
{
	bool hasData = false;

	IoBranch* branchId = *branches_[PathToUniqueString(path)];
	if (branchId != NULL && branchId->data.Size() > 0) {
		hasData = true;
	}

	bool hasSiblings = HasSiblings(path);

	return !hasData && !hasSiblings;
}

bool IoDataTree::Descendant(Vector<int> ancestor, Vector<int> descendant) const
{
	if (descendant.Size() <= ancestor.Size()) {
		return false;
	}

	for (unsigned i = 0; i < ancestor.Size(); ++i) {
		if (ancestor[i] != descendant[i]) {
			return false;
		}
	}

	return true;
}

///////////////////////////////////////
// Tree operations
///////////////////////////////////////

IoDataTree IoDataTree::Flatten() const
{
	HashMap<String, IoBranch*>::ConstIterator it;

	Vector<Variant> allVariants;

	for (it = branches_.Begin(); it != branches_.End(); ++it) {
		allVariants.Push(it->second_->data);
	}

	Vector<int> path;
	path.Push(0);

	IoDataTree simplifiedTree(GetContext());
	simplifiedTree.Add(path, allVariants);

	return simplifiedTree;
}

IoDataTree IoDataTree::Graft() const
{
	HashMap<String, IoBranch*>::ConstIterator it;

	IoDataTree graftedTree(GetContext());

	for (it = branches_.Begin(); it != branches_.End(); ++it) {
		Vector<Variant> vlist = it->second_->data;
		unsigned numItems = vlist.Size();
		String curPathString = it->first_;
		Vector<int> curPath = PathFromUniqueString(curPathString);
		if (numItems > 1) {
			Vector<int> nextBranch = GetNextNewBranchPath(curPath);
			for (unsigned i = 0; i < numItems; ++i) {
				Vector<int> pathToAdd = IncrementBranchPath(nextBranch, (int)i);
				graftedTree.Add(pathToAdd, vlist[i]);
			}
		}
		else {
			graftedTree.Add(curPath, vlist);
		}
	}

	return graftedTree;
}

IoDataTree IoDataTree::FlipMatrix() const
{
	HashMap<String, IoBranch*>::ConstIterator it;
	IoDataTree flippedTree(GetContext());
	int numElements = branches_.Begin()->second_->data.Size();
	for (it = branches_.Begin(); it != branches_.End(); ++it)
	{
		if (it->second_->data.Size() != numElements)
		{
			return flippedTree;
		}
	}

	//actually do the flip
	Vector<int> path;
	path.Push(0);
	path.Push(0);
	for (int i = 0; i < numElements; i++)
	{
		path[1] = i;
		for (it = branches_.Begin(); it != branches_.End(); ++it)
		{
			flippedTree.Add(path, it->second_->data[i]);
		}
	}

	return flippedTree;
}

void IoDataTree::FillInMissingPaths(Vector<int> path)
{
	Vector<int> copyPath = path;

	for (unsigned i = path.Size() - 1; i > 0; --i) {
		copyPath.Erase(i);
		String copyPathString = PathToUniqueString(copyPath);
		IoBranch* branch = branches_[copyPathString];
		if (branch == NULL) {
			IoBranch* newBranch = new IoBranch(copyPath);
			branches_[copyPathString] = newBranch;
		}
	}
}

void IoDataTree::FillInAllMissingPaths()
{
	HashMap<String, IoBranch*>::ConstIterator it;

	for (it = branches_.Begin(); it != branches_.End(); ++it) {
		String curPathString = it->first_;
		Vector<int> curPath = PathFromUniqueString(curPathString);
		FillInMissingPaths(curPath);
	}
}

void IoDataTree::ModifyPathInPlace(Vector<int> oldPath, Vector<int> newPath)
{
	String oldPathString = PathToUniqueString(oldPath);
	IoBranch* oldBranchId = branches_[oldPathString];
	if (oldBranchId == NULL) {
		URHO3D_LOGERROR("ERROR: IoDataTree::ModifyPath --- path to modify does not exist");
	}
	assert(oldBranchId != NULL);

	String newPathString = PathToUniqueString(newPath);
	IoBranch* newBranchId = branches_[newPathString];
	if (newBranchId != NULL) {
		URHO3D_LOGERROR("ERROR: IoDataTree::ModifyPath --- new path already exists");
	}
	assert(newBranchId == NULL);

	Vector<Variant> data = branches_[oldPathString]->data;
	delete oldBranchId;

	branches_.Erase(branches_.Find(oldPathString));
	newBranchId = new IoBranch(newPath);
	newBranchId->data = data;

	branches_[newPathString] = newBranchId;
}

IoDataTree IoDataTree::DeleteZeroSiblingPath(Vector<int> zeroSiblingPath) const
{
	IoDataTree copyTree = *this;

	IoBranch* thisBranch = copyTree.branches_[PathToUniqueString(zeroSiblingPath)];
	if (thisBranch != NULL) {
		assert(thisBranch->data.Size() == 0);
		// there is data at this branch, we cannot eliminate and should not be trying to
	}

	unsigned zeroSiblingIndex = zeroSiblingPath.Size() - 1;

	delete thisBranch;
	copyTree.branches_.Erase(copyTree.branches_.Find(PathToUniqueString(zeroSiblingPath)));

	HashMap<String, IoBranch*>::ConstIterator it;

	for (it = branches_.Begin(); it != branches_.End(); ++it) {
		String curPathString = it->first_;
		Vector<int> curPath = PathFromUniqueString(curPathString);

		if (Descendant(zeroSiblingPath, curPath)) {
			assert(curPath[zeroSiblingIndex] == 0);
			Vector<int> newCurPath = curPath;
			newCurPath.Erase(zeroSiblingIndex);
			copyTree.ModifyPathInPlace(curPath, newCurPath);
		}
	}

	return copyTree;
}

IoDataTree IoDataTree::Simplify() const
{
	IoDataTree tmpTree = *this;
	tmpTree.FillInAllMissingPaths();

	HashMap<String, IoBranch*>::ConstIterator it;

	bool done = false;

	while (!done) {

		done = true;

		for (it = tmpTree.branches_.Begin(); it != tmpTree.branches_.End(); ++it) {
			String curPathString = it->first_;
			Vector<int> curPath = PathFromUniqueString(curPathString);
			if (tmpTree.CanBeDeleted(curPath)) {
				// adjust curPath and all descendants accordingly
				IoDataTree copyTmpTree = tmpTree.DeleteZeroSiblingPath(curPath);
				tmpTree = copyTmpTree;
				done = false;
				break; // iterators invalidated!
			}
		}

		tmpTree.Begin();
	}

	IoDataTree simplifiedTree = tmpTree;
	return simplifiedTree;
}

// This function helps LocalSolve accomodate one-to-many components.
// To safely call this on a tree, the data lists "Vector<Variant> data"
// at each branch of the tree must all be such that each data[i] is either
//   a Variant of type VAR_VARIANTVECTOR
//   a Variant of type VAR_NONE
IoDataTree IoDataTree::OneToManyGraft() const
{
	IoDataTree graftedTree(GetContext());

	HashMap<String, IoBranch*>::ConstIterator it;

	for (it = branches_.Begin(); it != branches_.End(); ++it) {

		// setup "curPath" and the "data" stored there
		String curPathString = it->first_;
		Vector<int> curPath = PathFromUniqueString(curPathString);
		Vector<Variant> data = it->second_->data;

		if (data.Size() > 1) {
			Vector<int> basePath = GetNextNewBranchPath(curPath);
			for (unsigned i = 0; i < data.Size(); ++i) {
				Vector<int> newPath = IncrementBranchPath(basePath, (int)i);
				Variant variant = data[i];
				Vector<Variant> newData = variant.GetVariantVector();
				graftedTree.Add(newPath, newData);
			}
		}
		else if (data.Size() == 1) {
			if (data[0].GetType() == VariantType::VAR_VARIANTVECTOR) {

				Vector<Variant> newData = data[0].GetVariantVector();
				graftedTree.Add(curPath, newData);
			}
			else if (data[0].GetType() == VariantType::VAR_NONE) {
				graftedTree.Add(curPath, data);
			}
			else {
				URHO3D_LOGERROR("Unexpected tree structure crashed one-to-many component!");
				return IoDataTree(GetContext());
			}
		}
		else {
			// data is empty but still add the path, since the previous tree had the path
			graftedTree.Add(curPath, data);
		}
	}

	//std::cout << "Original Tree:\n";
	//std::cout << ToString().CString() << "\n";
	//std::cout << "Grafted Tree:\n";
	//std::cout << graftedTree.ToString().CString() << "\n";
	return graftedTree;
}


///////////////////////////////////////


// Returns true if childCandidate stores a path that is an immediate descendant of the path stored
// in parentCandidate, and false otherwise.
bool IoDataTree::IsParentChildPathPair(Vector<int> parentCandidate, Vector<int> childCandidate) const
{
	unsigned parentLength = parentCandidate.Size();
	unsigned childLength = childCandidate.Size();

	if (childLength != parentLength + 1) {
		return false;
	}

	for (unsigned i = 0; i < parentLength; ++i) {
		if (childCandidate[i] != parentCandidate[i]) {
			return false;
		}
	}

	return true;
}

Vector<int> IoDataTree::Begin()
{
	branchIterator_ = branches_.Begin();
	lastItemIndex_ = 0;
	branchOverflow_ = false;
	itemOverflow_ = false;
	return PathFromUniqueString(branchIterator_->first_);
}

Vector<int> IoDataTree::GetNextBranch()
{
	Vector<int> pathOut;
	if (++branchIterator_ == branches_.End())
	{
		branchOverflow_ = true;
		--branchIterator_;
	}
	lastItemIndex_ = 0;
	itemOverflow_ = false;
	pathOut = PathFromUniqueString(branchIterator_->first_);


	return pathOut;
}

Vector<int> IoDataTree::GetCurrentBranch() const
{
	return PathFromUniqueString(branchIterator_->first_);
}

Vector<int> IoDataTree::IncrementBranchPath(Vector<int> path, int incSize) const
{
	assert(path.Size() > 0);

	Vector<int> incrementedPath = path;
	int tail = incrementedPath[incrementedPath.Size() - 1];
	incrementedPath[incrementedPath.Size() - 1] = tail + incSize;

	return incrementedPath;
}

bool IoDataTree::AreSiblings(Vector<int> path1, Vector<int> path2) const
{
	if (path1.Size() != path2.Size()) {
		return false;
	}

	assert(path1.Size() == path2.Size());

	if (path1.Size() == 0) {
		URHO3D_LOGERROR("IoDataTree::AreSiblings called on empty path");
		return false;
	}

	assert(path1.Size() > 0);

	unsigned numIndices = path1.Size();
	for (unsigned i = 0; i < numIndices - 1; ++i) {
		if (path1[i] != path2[i]) {
			return false;
		}
	}

	if (path1[numIndices - 1] == path2[numIndices - 1]) {
		// paths are identical
		return false;
	}

	return true;
}

// Returns
//   true if path2 "witnesses" the existence of a sibling for path1
//   false otherwise.
// Exists to accomodate the possibility that we have empty nodes never added
// to the tree but whose existence is implicit in the existence of a descendant node that we have added.
// For example, this returns true for
//   {0;1;0;2;0}
//   {0;1;0;2;1;0;0;8}
// because the paths "witness" the existence of siblings branching from {0;1;0;2}.
// It returns false for
//   {0;1;0;2;0;1}
//   {0;1;0;2;1;0;0;8}
// because we only look for whether path2 witnesses the existence of a sibling for path1.
bool IoDataTree::WitnessSiblings(Vector<int> path1, Vector<int> path2) const
{
	if (path2.Size() < path1.Size()) {
		return false;
	}

	Vector<int> frontPath2(path1.Size());
	for (unsigned i = 0; i < frontPath2.Size(); ++i) {
		frontPath2[i] = path2[i];
	}

	return AreSiblings(path1, frontPath2);
}

bool EqualPaths(const Vector<int>& lhs, const Vector<int>& rhs)
{
	if (lhs.Size() != rhs.Size()) {
		return false;
	}

	for (unsigned i = 0; i < lhs.Size(); ++i) {
		if (lhs[i] != rhs[i]) {
			return false;
		}
	}

	return true;
}

bool ComparePaths(const Vector<int>& lhs, const Vector<int>& rhs)
{
	bool flag = false;

	if (lhs.Size() == 0) {
		if (rhs.Size() == 0) {
			flag = false;
		}
		else {
			flag = true;
		}
	}
	else if (rhs.Size() == 0) {
		flag = false;
	}
	else {
		if (lhs[0] < rhs[0]) {
			flag = true;
		}
		else if (lhs[0] > rhs[0]) {
			flag = false;
		}
		else { // lhs.GetIndexAt(0) == rhs.GetIndexAt(0)
			Vector<int> lhsCopy = lhs;
			Vector<int> rhsCopy = rhs;
			lhsCopy.Erase(0);
			rhsCopy.Erase(0);
			flag = ComparePaths(lhs, rhs);
		}
	}
	return flag;
}
