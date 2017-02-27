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