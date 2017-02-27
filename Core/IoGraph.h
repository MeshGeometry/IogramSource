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

#include <memory>
#include <vector>

#include "IoComponentBase.h"

class URHO3D_API IoGraph : public Urho3D::Object
{
	URHO3D_OBJECT(IoGraph, Urho3D::Object)
	
private:
	Urho3D::Vector<Urho3D::SharedPtr<IoComponentBase> > components_;
	Urho3D::Vector<bool> rootFlags_;

	

public:
	IoGraph(Urho3D::Context* context) : Urho3D::Object(context), components_(0), rootFlags_(0) {};

	//pointer to current scene
	Urho3D::Scene* scene;

	////////////////////////////////////////////////////
	////////////// SERIALIZATION ///////////////////////

	friend class IoSerialization;

	////////////////////////////////////////////////////
	////////////////////////////////////////////////////

	void StepGraph(){};

	void AddNewComponent();
	void AddNewComponent(Urho3D::SharedPtr<IoComponentBase>); // this is meant to be a newly constructed node, so just in case all connections are deleted

	void DeleteComponent(int index);
	void Clear();

	Urho3D::SharedPtr<IoComponentBase> GetComponentPtr(int index);
	int GetComponentIndex(Urho3D::String ID);

	void AddConnection(
		int parentIndex,
		int outputIndex,
		int childIndex,
		int inputIndex
	);

	void DeleteConnection(
		int parentIndex,
		int outputIndex,
		int childIndex, 
		int inputIndex
	);

	void SetInputIoDataTree(
		int componentIndex,
		int inputIndex,
		IoDataTree ioDataTree
	);

	unsigned GetOutputIoDataTree(
		unsigned componentIndex,
		unsigned outputIndex,
		IoDataTree& ioDataTree
	);

	void AddInputSlotToComponent(int componentIndex); // adds IoInputSlot to end of Vector, index data still valid
	void DeleteInputSlotFromComponent(int componentIndex, int inputIndex); // warning: changes index data unless inputIndex is top index

	int GetDummyNodeCount() { return (int)components_.Size(); }
	const Urho3D::Vector<Urho3D::SharedPtr<IoComponentBase> > GetAllComponents(){ return components_; };
	IoComponentBase* GetComponent(int id) { return components_.At(id).Get(); };
	int GetComponent(Urho3D::String ID);
	Urho3D::Vector<unsigned> GetDownstreamComponentIndices(unsigned i) const;

	void UpdateRoots();

	int SolveGraph();
	int TopoSolveGraph();
	int QuickTopoSolveGraph();
	int QuickSolveGraph();

	bool IsAcyclic(Urho3D::Vector<int>& top_nbr) const;
};