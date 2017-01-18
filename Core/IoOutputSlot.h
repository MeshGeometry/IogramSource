#pragma once

#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>

#include "IoComponentBase.h"
#include "IoDataTree.h"

class IoInputSlot;

class URHO3D_API IoOutputSlot : public Urho3D::Object {

	URHO3D_OBJECT(IoOutputSlot, Urho3D::Object)

public:
	IoOutputSlot(Urho3D::Context* context, Urho3D::SharedPtr<IoComponentBase> homeComponent) :
		Urho3D::Object(context),
		homeComponent_(homeComponent),
		linkedInputSlots_(0),
		ioDataTree_(context),
		dataAccess_(DataAccess::ITEM)
	{
	}

	IoOutputSlot(Urho3D::Context* context, Urho3D::SharedPtr<IoComponentBase> homeComponent, DataAccess dataAccess) :
		Urho3D::Object(context),
		homeComponent_(homeComponent),
		linkedInputSlots_(0),
		ioDataTree_(context),
		dataAccess_(dataAccess)
	{
	}

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

	////////////////////////////////////////////////////
	////////////// SERIALIZATION ///////////////////////

	friend class IoSerialization;

	////////////////////////////////////////////////////
	////////////////////////////////////////////////////

public:
	Urho3D::SharedPtr<IoComponentBase> const GetHomeComponent() { return homeComponent_; }
	Urho3D::Vector<Urho3D::SharedPtr<IoInputSlot> > GetLinkedInputSlots() const { return linkedInputSlots_; }
	unsigned GetNumLinkedInputSlots() const { return linkedInputSlots_.Size(); }
	Urho3D::SharedPtr<IoInputSlot> GetLinkedInputSlot(unsigned index) const;
	IoDataTree GetIoDataTree() const { return ioDataTree_; }
	void SetIoDataTree(IoDataTree ioDataTree);

	void Transmit();
	void Transmit(Urho3D::SharedPtr<IoInputSlot> in);

	DataAccess GetDataAccess() const { return dataAccess_; }
	void SetDataAccess(DataAccess dataAccess) { dataAccess_ = dataAccess; }

private:
	Urho3D::SharedPtr<IoComponentBase> homeComponent_;
	Urho3D::Vector<Urho3D::SharedPtr<IoInputSlot> > linkedInputSlots_;
	IoDataTree ioDataTree_;
	DataAccess dataAccess_;

private:
	Urho3D::String name_ = "";
	Urho3D::String variableName_ = "";
	Urho3D::String description_ = "";
	Urho3D::VariantType typeHint_ = Urho3D::VariantType::VAR_NONE;
	Urho3D::Variant defaultValue_ = Urho3D::Variant();
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
};