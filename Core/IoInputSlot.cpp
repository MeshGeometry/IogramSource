#include "IoInputSlot.h"

#include "IoComponentBase.h"
#include "NetworkUtilities.h"

using namespace Urho3D;

IoInputSlot::IoInputSlot(Urho3D::Context* context, Urho3D::SharedPtr<IoComponentBase> homeComponent, DataAccess dataAccess, Urho3D::Variant defaultValue) :
	Object(context),
	homeComponent_(homeComponent),
	linkedOutputSlot_(),
	ioDataTree_(context, defaultValue),
	dataAccess_(dataAccess),
	defaultValue_(defaultValue),
	isProtected_(true)
{
}

void IoInputSlot::HardSet(IoDataTree ioDataTree)
{
	::mDisconnect(Urho3D::SharedPtr<IoInputSlot>(this));
	ioDataTree_ = ioDataTree;
	homeComponent_->solvedFlag_ = 0;
}

void IoInputSlot::SoftSet(IoDataTree ioDataTree)
{
	ioDataTree_ = ioDataTree;
	homeComponent_->solvedFlag_ = 0;
}

// Depends on defaultValue_ having been set (or uses default defaultValue_).
// Does not break connections.
void IoInputSlot::DefaultSet()
{
	ioDataTree_ = IoDataTree(GetContext(), defaultValue_);
	homeComponent_->solvedFlag_ = 0;
}

void IoInputSlot::Lose()
{
	::mDisconnect(Urho3D::SharedPtr<IoInputSlot>(this));
	ioDataTree_ = IoDataTree(GetContext(), defaultValue_);
	homeComponent_->solvedFlag_ = 0;
}

IoDataTree* IoInputSlot::GetIoDataTreePtr()
{
	return &ioDataTree_;
}

void IoInputSlot::SetDefaultValueSafe(Urho3D::Variant defaultValue)
{
	if (dataAccess_ == DataAccess::LIST)
	{
		VariantVector varList;
		varList.Push(defaultValue);
		defaultValue_ = varList;
	}
	else
	{
		defaultValue_ = defaultValue;
	}
}