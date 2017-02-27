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


#include "IoGraph.h"

#include <iostream>
#include <memory>
#include <vector>

#include "IndexUtilities.h"

using namespace Urho3D;

// Finds a topological sorting of the graph, i.e.
// Enumerates the vertices 1,..., n such that 
// i < j for each edge ij
// returns a boolean which says whether the graph is acyclic or not.
// See page 50, "Graphs, Networks and Algorithms" by Dieter Jungnickel
// https://books.google.ca/books?id=PrXxFHmchwcC&pg=PA92&redir_esc=y#v=snippet&q=topological%20sorting&f=false
// top_nbr is a vector of ints representing vertex index. 
// top_nbr[i] is the index of the vertex with topological sort number i
bool  IoGraph::IsAcyclic(Vector<int>& top_nbr) const
{
	// initialize the sorting index
	int N = 0;
	int n = components_.Size();

	// in_degrees is a record of the in_degrees of the nodes
	// we will modify this list, which is why we don't use built-in in-degree info.
	Vector<int> in_degrees;
	for (int i = 0; i < n; ++i)
	{
		in_degrees.Push(components_[i]->ComputeUniqueInDegree());
	//	std::cout << "component " << i << " has in degree " << in_degrees[i] << std::endl;
	}

	// roots is a list of roots at the current step, 
	Vector<IoComponentBase*> roots;

	// Add all root vertices to sorted list
	for (int i = 0; i < n; ++i)
	{
		if (in_degrees[i] == 0)
			roots.Push(components_[i].Get());
	}

	while (!roots.Empty())
	{
		// grab the first vertex in roots list
		IoComponentBase* firstVert = roots[0];

		// find its index
		int vertID = -1;
		for (int i = 0; i < (int)components_.Size(); ++i)
		{
			if (firstVert == components_[i].Get())
				vertID = i;
		}

		// assign current topo_number to this vertex
		top_nbr.Push(vertID); 
		N += 1;
		// then remove that vertex from the list
		roots.Erase(roots.Begin());

		// get children of the current root node:
		Vector<SharedPtr<IoComponentBase> > children = firstVert->GetUniqueComponentsOut();
	//	std::cout << "children of vertex " << vertID << " are: " << std::endl;
		for (unsigned j = 0; j < children.Size(); ++j)
		{
			// null pointers are not children
			if (children[j].Null())
				continue;

			// find indices of the children of the current root node: 
			int idx = -1;
			for (int i = 0; i < (int)components_.Size(); ++i)
			{
				if (children[j] == components_[i].Get())
					idx = i;
			}
			//std::cout << idx << ", ";

			// deprecate their in-degree, add to roots list
			in_degrees[idx] = in_degrees[idx] - 1;
			if (in_degrees[idx] == 0)
				roots.Push(children[j]);
		}
		//std::cout << std::endl;
	}
	if (N == n) {
		// the graph contains no directed cycle
		return true;
	}
	else {
		// if N < n then we have detected a cycle
		return false;
	}
}



// alternate graph solver
int IoGraph::TopoSolveGraph()
{
	int numSolved = 0;
	VariantVector solvedIndices;
	Vector<int> top_number;
	bool goodToSolve = IsAcyclic(top_number);
	// if it contains a cycle, return failure
	if (!goodToSolve)
		return 0;

	// walk through the nodes according to their topological sort index
	for (unsigned i = 0; i < top_number.Size(); ++i)
	{
		// Only tries to call LocalSolve if solve is enabled
		if (components_[top_number[i]]->IsSolveEnabled()) {
			components_[top_number[i]]->LocalSolve();
			bool solveFlag = components_[top_number[i]]->IsSolved();
			if (solveFlag) {
				solvedIndices.Push(top_number[i]);
			}
		}
		// Regardless of whether LocalSolve was called,
		// checks component for input/output consistency
		// (solveFlag == true if and only if solveFlag_ == 1).
		bool solveFlag = components_[top_number[i]]->IsSolved();
		if (solveFlag) {
			++numSolved;
		}
	}

	//send message that graph has been solved
	VariantMap data;
	data["graph"] = this;
	data["indices"] = solvedIndices;
	SendEvent("OnSolveGraph", data);

	if (numSolved == components_.Size()) {
		std::cout << "Graph TopoSolved!!!" << std::endl;
		return 1;
	}
	else
		return 0;

}

// same as TopoSolve, but checks flags and only solves components flagged as unsolved.
int IoGraph::QuickTopoSolveGraph()
{
	int numSolved = 0;
	VariantVector solvedIndices;
	Vector<int> top_number;
	bool goodToSolve = IsAcyclic(top_number);
	// if it contains a cycle, return failure
	if (!goodToSolve)
		return 0;

	// walk through the nodes according to their topological sort index
	for (unsigned i = 0; i < top_number.Size(); ++i)
	{
		// if the component is unsolved and solve is enabled, then solve
		if (
			!components_[top_number[i]]->IsSolved() &&
			components_[top_number[i]]->IsSolveEnabled()
			) {
			int solveFlag = components_[top_number[i]]->LocalSolve();
			if (solveFlag == 1) {
				++numSolved;
				solvedIndices.Push(top_number[i]);
			}
		}
		// otherwise check if component has been solved at previous stage
		else if (components_[top_number[i]]->IsSolved()) {
			++numSolved;
		}
	}

	//send message that graph has been solved
	VariantMap data;
	data["graph"] = this;
	data["indices"] = solvedIndices;
	SendEvent("OnSolveGraph", data);

	if (numSolved == components_.Size()) 
		return 1;
	else
		return 0;
}


//////////////////////////////////////////////////////////////////


// POTENTIAL PROBLEM NOW
void IoGraph::AddNewComponent()
{
	SharedPtr<IoComponentBase> nodePtr(new IoComponentBase(GetContext(), 2, 1)); // 2 inputs, 1 output by default (following Grasshopper)
	components_.Push(nodePtr);
	rootFlags_.Push(true);
}

// assumption is that this should be a newly constructed node, so all connections are deleted just in case it isn't
void IoGraph::AddNewComponent(SharedPtr<IoComponentBase> component)
{
	//component->DisconnectAllChildren();
	//component->DisconnectAllParents();
	components_.Push(component);
	rootFlags_.Push(true);
}

void IoGraph::AddConnection(
	int parentIndex,
	int outputIndex,
	int childIndex,
	int inputIndex
	)
{
	// assuming argument validity do this:
	components_[parentIndex]->ConnectChild(
		components_[childIndex], // change this once IoComponentBase uses shared_ptr's too!
		inputIndex,
		outputIndex
		);

	// child is now definitely not a root
	rootFlags_[childIndex] = false;
}

void IoGraph::DeleteConnection(
	int parentIndex,
	int outputIndex,
	int childIndex,
	int inputIndex
	)
{
	// assuming validity of arguments
	components_[parentIndex]->DisconnectChild(outputIndex);

	// perhaps child is now a root....
	if (components_[childIndex]->ComputeInDegree() == 0) {
		rootFlags_[childIndex] = true;
	}

}

void IoGraph::DeleteComponent(int index)
{
	SharedPtr<IoComponentBase> ptr = components_[index];
	components_[index]->DisconnectAllChildren();
	components_[index]->DisconnectAllParents();

	components_.Erase(components_.Begin() + index);
	rootFlags_.Erase(rootFlags_.Begin() + index);

	// ALERT: there may be new roots now. 
	UpdateRoots();

}

void IoGraph::Clear()
{
	for (unsigned i = 0; i < components_.Size(); ++i) {
		components_[i]->UnsubscribeFromAllEvents();
	}
	components_.Clear();
	rootFlags_.Clear();
}

void IoGraph::AddInputSlotToComponent(int component)
{
	components_[component]->AddInputSlot();
}

void IoGraph::DeleteInputSlotFromComponent(int component, int inputIndex)
{
	components_[component]->DeleteInputSlot(inputIndex);
}

int IoGraph::QuickSolveGraph()
{
	int numSolved = 0;
	int numToSolve = (int)components_.Size();

	// attempt to solve the roots
	for (int i = 0; i < (int)rootFlags_.Size(); ++i) {
		if (rootFlags_[i]) {
			//std::cout << "solving root with index = " << i << std::endl;
			if (!components_[i]->IsSolved())
			{
				int solveFlag = components_[i]->LocalSolve();
				if (solveFlag == 1) {
					++numSolved;
				}
			}
			else
			{
				++numSolved;
			}
		}
	}

	if (numSolved == numToSolve) {
		// all nodes are roots having all inputs set
		VariantMap data;
		data["graph"] = this;
		SendEvent("OnSolveGraph", data);
		return 1;
	}

	// if here, numSolved < numToSolve
	while (numSolved < numToSolve) {

		// loop over all nodes and try to solve the unsolved ones
		int numSolvedThisPass = 0;
		for (int i = 0; i < (int)components_.Size(); ++i) {
			if (!components_[i]->IsSolved()) {
				//std::cout << "solving component with index = " << i << std::endl;

				int solveFlag = components_[i]->LocalSolve();
				if (solveFlag == 1) {
					++numSolved;
					++numSolvedThisPass;
				}
			}
			else
			{
				++numSolved;
				++numSolvedThisPass;
			}
		}

		// check status
		if (numSolvedThisPass == 0) {
			// all nodes are roots having all inputs set
			VariantMap data;
			data["graph"] = this;
			SendEvent("OnSolveGraph", data);
			return 0;
		}
	}

	//send message that graph has been solved
	VariantMap data;
	data["graph"] = this;
	SendEvent("OnSolveGraph", data);

	// if here, numSolved == numToSolve
	return 1;
}

int IoGraph::SolveGraph()
{
	int numSolved = 0;
	int numToSolve = (int)components_.Size();

	// attempt to solve the roots
	for (int i = 0; i < (int)rootFlags_.Size(); ++i) {
		if (rootFlags_[i]) {
			//std::cout << "solving root with index = " << i << std::endl;
			int solveFlag = components_[i]->LocalSolve();
			if (solveFlag == 1) {
				++numSolved;
			}
		}
	}

	if (numSolved == numToSolve) {
		// all nodes are roots having all inputs set
		VariantMap data;
		data["graph"] = this;
		SendEvent("OnSolveGraph", data);
		return 1;
	}

	// if here, numSolved < numToSolve
	while (numSolved < numToSolve) {

		// loop over all nodes and try to solve the unsolved ones
		int numSolvedThisPass = 0;
		for (int i = 0; i < (int)components_.Size(); ++i) {
			if (!components_[i]->IsSolved()) {
				//std::cout << "solving component with index = " << i << std::endl;
				int solveFlag = components_[i]->LocalSolve();
				if (solveFlag == 1) {
					++numSolved;
					++numSolvedThisPass;
				}
			}
		}

		// check status
		if (numSolvedThisPass == 0) {
			return 0;
		}
	}

	//send message that graph has been solved
	VariantMap data;
	data["graph"] = this;
	SendEvent("OnSolveGraph", data);

	// if here, numSolved == numToSolve
	return 1;
}

void IoGraph::SetInputIoDataTree(
	int componentIndex,
	int inputIndex,
	IoDataTree ioDataTree
	)
{
	components_[componentIndex]->InputHardSet(inputIndex, ioDataTree);
	UpdateRoots();
}


unsigned IoGraph::GetOutputIoDataTree(
	unsigned componentIndex,
	unsigned outputIndex,
	IoDataTree& ioDataTree
	)
{
	assert(IndexInRange(componentIndex, components_.Size()));
	assert(IndexInRange(outputIndex, components_[componentIndex]->GetNumOutputs()));

	ioDataTree = components_[componentIndex]->GetOutputIoDataTree(outputIndex);

	return 1;
}

void IoGraph::UpdateRoots()
{
	for (unsigned i = 0; i < components_.Size(); ++i)
	{
		if (components_[i]->ComputeInDegree() == 0)
			rootFlags_[i] = true;
		else
			rootFlags_[i] = false;
	}
}


SharedPtr<IoComponentBase> IoGraph::GetComponentPtr(int index)
{
	assert(IndexInRange(index, (int)components_.Size()));
	return components_[index];
}

int IoGraph::GetComponentIndex(String ID)
{
	for (int i = 0; i < (int)components_.Size(); i++)
	{
		if (components_[i]->ID == ID)
		{
			return i;;
		}
	}

	return -1;
}

int IoGraph::GetComponent(String ID)
{
	for (int i = 0; i < (int)components_.Size(); i++)
	{
		if (components_[i]->ID == ID)
		{
			return i;
		}
	}

	return -1;
}


// i: index into IoGraph::components_ of query component
// Returns:
//   List of indices into IoGraph::components_ of components immediately downstream of query component
Vector<unsigned> IoGraph::GetDownstreamComponentIndices(unsigned i) const
{
	assert(IndexInRange((int)i, (int)components_.Size()));

	Vector<SharedPtr<IoComponentBase> > downstreamComps = components_[i]->GetUniqueComponentsOut();

	Vector<unsigned> downIndices;
	// for each unique downstream component,
	// look up what its index is in IoGraph::components_ and Push that index to downIndices
	for (unsigned i = 0; i < downstreamComps.Size(); ++i) {
		SharedPtr<IoComponentBase> downComp = downstreamComps[i];
		for (unsigned j = 0; j < components_.Size(); ++j) {
			if (downComp == components_[j]) {
				downIndices.Push(j);
				break;
			}
		}
	}

	if (downIndices.Size() != downstreamComps.Size()) {
		std::cout << "something strange has happened" << std::endl;
	}

	return downIndices;
}