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

#include <vector>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Context.h>

#include <Urho3D/UI/UIElement.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>

#include <Urho3D/IO/Log.h>

#include "IoComponentGroup.h"
#include "IoDataTree.h"
#include "IoInputSlot.h"
#include "IoOutputSlot.h"

//////////////////
// IoComponentBase

class URHO3D_API IoComponentBase : public Urho3D::Object {

	URHO3D_OBJECT(IoComponentBase, Urho3D::Object)

public:
	IoComponentBase(Urho3D::Context* context) : IoComponentBase(context, 2, 1) { };
	IoComponentBase(Urho3D::Context* context, int numInputs, int numOutputs);

	virtual ~IoComponentBase() {}

	// disable copy constructor and assignment operator
	IoComponentBase(const IoComponentBase&) = delete;
	void operator=(const IoComponentBase&) = delete;

	friend class IoInputSlot;
	friend class IoOutputSlot;
	friend class IoSerialization;
	friend class Editor_NodeView;

public:
	virtual int NewLocalSolve();
	virtual int OldLocalSolve();
	virtual int LocalSolve();
	virtual void ClearOutputs();
	virtual void PreLocalSolve() {};
	virtual void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);
	bool IsSolved() const { return solvedFlag_ == 1; }

	void InputHardSet(int inputIndex, IoDataTree ioDataTree);

	IoDataTree GetOutputIoDataTree(unsigned index);

	///slot manipulation
	void AddInputSlot();
	void AddOutputSlot();
	void DeleteInputSlot(int index);
	void DeleteOutputSlot(int index);

	///slot convenience functions
	IoInputSlot* AddInputSlot(
		Urho3D::String name,
		Urho3D::String variableName,
		Urho3D::String description,
		Urho3D::VariantType type,
		DataAccess dataAccess);

	IoInputSlot* AddInputSlot(
		Urho3D::String name,
		Urho3D::String variableName,
		Urho3D::String description,
		Urho3D::VariantType type,
		DataAccess dataAccess,
		Urho3D::Variant defaultValue);

	IoOutputSlot* AddOutputSlot(
		Urho3D::String name,
		Urho3D::String variableName,
		Urho3D::String description,
		Urho3D::VariantType type,
		DataAccess dataAccess);

	///returns link info 
	Urho3D::Pair<Urho3D::SharedPtr<IoComponentBase>, int> GetIncomingLink(int inputSlotIdx);
	Urho3D::Pair<Urho3D::SharedPtr<IoComponentBase>, int> GetOutgoingLink(int outputSlotIdx, int linkIdx);

	///data about the component
	int GetNumInputs() const { return (int)inputSlots_.Size(); }
	int GetNumOutputs() const { return (int)outputSlots_.Size(); }
	int GetNumOutgoingLinks(int outputSlotIdx) const;
	int GetNumIncomingLinks(int inputSlotIdx) const;
	int GetInputSlotIndex(Urho3D::SharedPtr<IoInputSlot> inputSlot) const;
	int GetOutputSlotIndex(Urho3D::SharedPtr<IoOutputSlot> outputSlot) const;
	Urho3D::String GetIconTexture();

	Urho3D::String GetInputSlotName(int index) const;
	Urho3D::String GetInputSlotVariableName(int index) const;
	Urho3D::String GetInputSlotDescription(int index) const;
	Urho3D::VariantType GetInputSlotVariantType(int index) const;

	Urho3D::String GetOutputSlotName(int index) const;
	Urho3D::String GetOutputSlotVariableName(int index) const;
	Urho3D::String GetOutputSlotDescription(int index) const;
	Urho3D::VariantType GetOutputSlotVariantType(int index) const;

	/// connectivity
	void ConnectChild(Urho3D::SharedPtr<IoComponentBase> child, int indexIntoChildInput, int indexIntoParentOutput);
	void DisconnectChild(int indexIntoParentOutput);
	void DisconnectAllChildren();
	void DisconnectParent(int indexIntoChildInput);
	void DisconnectAllParents();
	int ComputeInDegree() const;
	int ComputeOutDegree() const;
	Urho3D::Vector<Urho3D::SharedPtr<IoComponentBase> > GetUniqueComponentsIn() const;
	Urho3D::Vector<Urho3D::SharedPtr<IoComponentBase> > GetUniqueComponentsOut() const;
	unsigned ComputeUniqueInDegree() const;
	unsigned ComputeUniqueOutDegree() const;

	std::vector<Urho3D::SharedPtr<IoComponentBase> > GetChildren();

	/* late metadata accessors and setters */
	Urho3D::String GetName() const { return name_; }
	void SetName(Urho3D::String name) { name_ = name; }
	Urho3D::String GetFullName() const { return fullName_; }
	void SetFullName(Urho3D::String fullName) { fullName_ = fullName; }
	Urho3D::String GetDescription() const { return description_; }
	void SetDescription(Urho3D::String description) { description_ = description; }
	IoComponentGroup GetGroup() const { return group_; }
	void SetGroup(IoComponentGroup group) { group_ = group; }
	Urho3D::String GetSubgroup() const { return subgroup_; }
	void SetSubgroup(Urho3D::String subgroup) { subgroup_ = subgroup; }

	// Flags for the renderer to check before rendering
	void EnablePreview() { previewEnabled_ = 1; }
	void DisablePreview() { previewEnabled_ = 0; }
	bool IsPreviewEnabled() const { return previewEnabled_ == 1; }

	// Flags for topological ordering-based solve methods
	void EnableSolve() { solveEnabled_ = 1; solvedFlag_ = 0; }
	void DisableSolve() { solveEnabled_ = 0; solvedFlag_ = 0; }
	bool IsSolveEnabled() const { return solveEnabled_ == 1; }

	//base functions for handling custom ui
	virtual Urho3D::String GetNodeStyle();
	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);
	bool SetGenericData(Urho3D::String key, Urho3D::Variant data);
	Urho3D::Variant GetGenericData(Urho3D::String key);
	Urho3D::String GetUniqueViewName() const { return uniqueViewID_; };
	void SetUniqueViewName(Urho3D::String viewName) { uniqueViewID_ = viewName; };
	Urho3D::IntVector2 GetCoordinates() const { return coordinates_; };
	void SetCoordinates(Urho3D::IntVector2 newCoords) { coordinates_ = newCoords; };
	Urho3D::IntVector2 GetViewSize() const { return viewSize_; };
	void SetViewSize(Urho3D::IntVector2 newSize) { viewSize_ = newSize; };

public:
	/* early metadata */
	//other identifying info
	Urho3D::String ID;
	Urho3D::String Name;
	Urho3D::String type;
	static Urho3D::String iconTexture;
	static Urho3D::String tags;


//protected: // protected so derived objects can access
	Urho3D::Vector<Urho3D::SharedPtr<IoInputSlot> > inputSlots_;
	Urho3D::Vector<Urho3D::SharedPtr<IoOutputSlot> > outputSlots_;

protected: // protected so derived objects can access
	// checks for valid input
	Urho3D::String VariantTypeToString(Urho3D::VariantType variantType) const;
	bool IsInputValid(unsigned inputIndex, const Urho3D::Variant& inputValue) const;
	bool IsAllInputValid(const Urho3D::Vector<Urho3D::Variant>& inSolveInstance) const;
	void SetAllOutputsNull(Urho3D::Vector<Urho3D::Variant>& outSolveInstance);

	int solvedFlag_;
	/*
	solvedFlag_ ==
	  1: indicates that this component has outputs computed from the current inputs
	  0: indicates that the above condition may not hold, because, for example:
	       -- the component may lack some input values
		   -- the outputs may disagree with what would be computed from the inputs
	  TO DISCUSS: behaviour upon instantiation!
	  EDIT: think of this as a LOCALsolvedFlag_, i.e., it refers specifically to whether LocalSolve has been able to call SolveInstance for all inputs
	*/

	/*
	LocalSolve will initially set this flag to 0.
	Then, if SolveInstance encounters input of wrong or VAR_NONE type, or logically subversive input like division by 0,
	it will set this flag to 1.
	Proper behavior for this flag is therefore ONGOING because it depends on every component's SolveInstance remembering to set this when appropriate.
	*/
	int warningFlag_ = 0;

	// 1: Flags this component as OK to preview -- does not register for rendering.
	// 0: Flags this component to be skipped by renderer, even if it is registered.
	int previewEnabled_ = 1;

	// 0: Flags this component as NOT to be solved.
	// 1: Flags this component as OK to solve.
	int solveEnabled_ = 1;

	/* later metadata */
	Urho3D::String name_ = "";
	Urho3D::String fullName_ = "";
	Urho3D::String description_ = "";
	IoComponentGroup group_ = IoComponentGroup::NONE;
	Urho3D::String subgroup_ = "";
	Urho3D::Vector<Urho3D::String> tags_;

	Urho3D::String uniqueViewID_;
	Urho3D::IntVector2 coordinates_;
	Urho3D::IntVector2 viewSize_;

	Urho3D::HashMap<Urho3D::String, Urho3D::Pair<Urho3D::String, Urho3D::Variant>> metaData_;
};