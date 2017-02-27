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

#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>

#include "IoDataTree.h"

class IoComponentBase;
class IoOutputSlot;

class URHO3D_API IoInputSlot : public Urho3D::Object {

	URHO3D_OBJECT(IoInputSlot, Urho3D::Object)

public:
	IoInputSlot(Urho3D::Context* context, Urho3D::SharedPtr<IoComponentBase> homeComponent) :
		Urho3D::Object(context),
		homeComponent_(homeComponent),
		linkedOutputSlot_(),
		ioDataTree_(context, Urho3D::Variant()),
		dataAccess_(DataAccess::ITEM)
	{
	}

	IoInputSlot(Urho3D::Context* context, Urho3D::SharedPtr<IoComponentBase> homeComponent, DataAccess dataAccess) :
		Urho3D::Object(context),
		homeComponent_(homeComponent),
		linkedOutputSlot_(),
		ioDataTree_(context, Urho3D::Variant()),
		dataAccess_(dataAccess)
	{
	}

	IoInputSlot(Urho3D::Context* context, Urho3D::SharedPtr<IoComponentBase> homeComponent, DataAccess dataAccess, Urho3D::Variant defaultValue);

	friend void mConnect(
		Urho3D::SharedPtr<IoOutputSlot> out,
		Urho3D::SharedPtr<IoInputSlot> in
	);
	friend void mDisconnect(Urho3D::SharedPtr<IoInputSlot> in);
	friend void mDisconnect(Urho3D::SharedPtr<IoOutputSlot> out);
	friend void mDisconnect(
		Urho3D::SharedPtr<IoOutputSlot> out,
		int indexIntoOut,
		Urho3D::SharedPtr<IoInputSlot> in
	);

	friend class IoOutputSlot;

	////////////////////////////////////////////////////
	////////////// SERIALIZATION ///////////////////////

	friend class IoSerialization;

	////////////////////////////////////////////////////
	////////////////////////////////////////////////////

public:

	Urho3D::SharedPtr<IoComponentBase> const GetHomeComponent() { return homeComponent_; }
	Urho3D::SharedPtr<IoOutputSlot> const GetLinkedOutputSlot() { return linkedOutputSlot_; }

	void HardSet(IoDataTree ioDataTree);
	void SoftSet(IoDataTree ioDataTree);
	void DefaultSet();
	void Lose();

	IoDataTree* GetIoDataTreePtr();
	IoDataTree GetIoDataTree() const { return ioDataTree_; }
	bool HasNoData() const { return ioDataTree_.IsEmptyTree(); }

	DataAccess GetDataAccess() const { return dataAccess_; }
	void SetDataAccess(DataAccess dataAccess) { dataAccess_ = dataAccess; }
	bool GetIsProtected() { return isProtected_; };
	void SetIsProtected( bool protect) { isProtected_ = protect; };

private: // protected?

private:
	Urho3D::SharedPtr<IoComponentBase> homeComponent_;
	Urho3D::SharedPtr<IoOutputSlot> linkedOutputSlot_;
	IoDataTree ioDataTree_;
	DataAccess dataAccess_;

private:
	// exploratory code
	// for now, these store completely free metadata to help the user (see e.g. Series component)
	// Since they do not interact with class invariants, they are not set by the constructor but by simple member set functions.
	// variantType_ conceivably could interact with code logic down the road.
	Urho3D::String name_ = "";
	Urho3D::String variableName_ = "";
	Urho3D::String description_ = "";
	Urho3D::VariantType typeHint_ = Urho3D::VariantType::VAR_NONE;
	Urho3D::Variant defaultValue_ = Urho3D::Variant();
	bool isProtected_ = true;

public:
	Urho3D::String GetName() const { return name_; }
	Urho3D::String GetVariableName() const { return variableName_; }
	Urho3D::String GetDescription() const { return description_; }
	Urho3D::VariantType GetVariantType() const { return typeHint_; }
	Urho3D::Variant GetDefaultValue() const { return defaultValue_; }
	void SetName(Urho3D::String name) { name_ = name; }
	void SetVariableName(Urho3D::String variableName) { variableName_ = variableName; }
	void SetDescription(Urho3D::String description) { description_ = description; }
	void SetVariantType(Urho3D::VariantType typeHint) { typeHint_ = typeHint; }
	void SetDefaultValue(Urho3D::Variant defaultValue) { defaultValue_ = defaultValue; }
	void SetDefaultValueSafe(Urho3D::Variant defaultValue);
};