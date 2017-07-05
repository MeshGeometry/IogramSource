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


#pragma once

#include <Urho3D/Container/Vector.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>

///determines the stride with which to iterate over the tree
enum DataAccess
{
	ITEM,
	LIST,
	TREE
};

///container for data at a specific address
class IoBranch
{
public:
	Urho3D::Vector<int> address;
	Urho3D::Vector<Urho3D::Variant> data;

public:
	IoBranch(Urho3D::Vector<int> target)
	{
		address = target;
	}
};

///all slots receive and output a datatree
class URHO3D_API IoDataTree : public Urho3D::Object
{
	URHO3D_OBJECT(IoDataTree, Urho3D::Object)
private:
	//the branches
	Urho3D::HashMap<Urho3D::String, IoBranch*> branches_;

	//Not totally sure about this, but basically need custom iteration logic
	Urho3D::HashMap<Urho3D::String, IoBranch*>::ConstIterator branchIterator_;
	int lastItemIndex_ = 0;

	//flags that track if iterator is in overflow mode
	//we can use these to avoid manually calculating which tree is the longest
	bool branchOverflow_ = false;
	bool itemOverflow_ = false;

	// use with caution
	void FillInMissingPaths(Urho3D::Vector<int> path);
	void FillInAllMissingPaths();
	void ModifyPathInPlace(Urho3D::Vector<int> oldPath, Urho3D::Vector<int> newPath);

public:
	// constructors, destructors, operator=
	IoDataTree(Urho3D::Context* context) : Urho3D::Object(context) {};
	// convenience constructors, for tests
	IoDataTree(Urho3D::Context* context, Urho3D::Variant item);
	IoDataTree(Urho3D::Context* context, Urho3D::Vector<Urho3D::Variant> items);
	~IoDataTree();
	IoDataTree(const IoDataTree&);
	IoDataTree& operator=(const IoDataTree& rhs);

	//serialization
	friend class IoSerialization;

	// operations that change the tree's state
	void Add(Urho3D::Vector<int> path, Urho3D::Variant item);
	void Add(Urho3D::Vector<int> path, Urho3D::VariantVector list);

	// Assumption:
	//   All Variants stored anywhere in tree have same VariantType, and in addition if they
	//   are VAR_VARIANTMAPS storing a custom type (e.g., TriMesh) all Variants share that custom type too.
	// This function copies one data item (no guarantee which one) onto dataOut.
	// The caller can then investigate dataOut manually to extract type information.
	// Finding type stored in an IoDataTree is proving to be error prone:
	// this functions aims to aid that lookup, without changing state of the tree and
	// without crashing. Hopefully!
	void LookupType(Urho3D::Variant& dataOut, DataAccess accessType) const;

	void GetNextItem(Urho3D::Variant& data, DataAccess accessType);
	Urho3D::Vector<int> GetNextBranch();
	Urho3D::Vector<int> Begin();

	// User-controlled tree operations
	IoDataTree Graft() const;
	IoDataTree Flatten() const;
	IoDataTree Simplify() const;
	IoDataTree FlipMatrix() const;

	// Automated tree operations
	IoDataTree OneToManyGraft() const;

	Urho3D::VariantMap ToVariantMap() const;

public:
	// Part of public interface: const operations with output depending on state
	void GetItem(Urho3D::Variant& item, Urho3D::Vector<int> path, int index) const;
	unsigned GetNumItemsAtBranch(Urho3D::Vector<int> path, DataAccess accessType) const;
	int GetNumBranches() const { return branches_.Keys().Size(); };
	Urho3D::Vector<int> GetCurrentBranch() const;
	Urho3D::String ToString(bool truncate=false) const;
	Urho3D::Vector<Urho3D::String> GetContent();
	bool branchOverflow() const { return branchOverflow_; };
	bool itemOverflow() const { return itemOverflow_; };
	bool IsEmptyTree() const { return branches_.Size() == 0; }
private:
	// const operations with output depending on state
	Urho3D::Vector<Urho3D::Vector<int> > FindChildPaths(Urho3D::Vector<int> path) const;
	Urho3D::Vector<int> GetNextNewBranchPath(Urho3D::Vector<int> path) const;
	bool HasSiblings(Urho3D::Vector<int> path) const;
	bool CanBeDeleted(Urho3D::Vector<int> path) const;
	bool Descendant(Urho3D::Vector<int> ancestor, Urho3D::Vector<int> descendant) const;
	IoDataTree DeleteZeroSiblingPath(Urho3D::Vector<int> zeroSiblingPath) const;

public:
	Urho3D::Vector<int> PathFromUniqueString(Urho3D::String pathString) const;
private:
	// purely functional const operations, output has no dependence on object state
	Urho3D::String PathToUniqueString(Urho3D::Vector<int> path) const;
	bool IsParentChildPathPair(Urho3D::Vector<int> parentCandidate, Urho3D::Vector<int> childCandidate) const;
	Urho3D::Vector<int> IncrementBranchPath(Urho3D::Vector<int> path, int incSize) const;
	bool AreSiblings(Urho3D::Vector<int> path1, Urho3D::Vector<int> path2) const;
	bool WitnessSiblings(Urho3D::Vector<int> path1, Urho3D::Vector<int> path2) const;
};

bool ComparePaths(const Urho3D::Vector<int>& lhs, const Urho3D::Vector<int>& rhs);
bool EqualPaths(const Urho3D::Vector<int>& lhs, const Urho3D::Vector<int>& rhs);