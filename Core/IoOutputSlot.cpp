#include "IoOutputSlot.h"

#include "IndexUtilities.h"
#include "IoInputSlot.h"

using namespace Urho3D;

Urho3D::SharedPtr<IoInputSlot> IoOutputSlot::GetLinkedInputSlot(unsigned index) const
{
	assert(IndexInRange(index, linkedInputSlots_.Size()));

	return linkedInputSlots_[index];
}

void IoOutputSlot::SetIoDataTree(IoDataTree ioDataTree)
{
	ioDataTree_ = ioDataTree;
	Transmit();
}

void IoOutputSlot::Transmit()
{
	for (unsigned i = 0; i < linkedInputSlots_.Size(); ++i) {
		linkedInputSlots_[i]->SoftSet(ioDataTree_);
	}
}

void IoOutputSlot::Transmit(Urho3D::SharedPtr<IoInputSlot> in)
{
	assert(in.NotNull());
	assert(in->linkedOutputSlot_ == Urho3D::SharedPtr<IoOutputSlot>(this));

	in->SoftSet(ioDataTree_);
}