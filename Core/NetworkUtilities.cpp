#include "NetworkUtilities.h"

#include "IndexUtilities.h"

void mConnect(
	Urho3D::SharedPtr<IoOutputSlot> out,
	Urho3D::SharedPtr<IoInputSlot> in
)
{
	assert(out.NotNull());
	assert(in.NotNull());

	assert(out->homeComponent_ != in->homeComponent_);

	if (in->linkedOutputSlot_ != out) {
		// in may have a connection established, but it is with an output other than out; therefore, disconnect is safe w.r.t. possible horrible index confusion
		mDisconnect(in);

		out->linkedInputSlots_.Push(in);
		in->linkedOutputSlot_ = out;

		in->ioDataTree_ = out->ioDataTree_;
		out->Transmit(in);
	}
	else {
		// this connection is already established. not totally sure what to do. probably nothing. or retransmit value anyway.
	}
}

void mDisconnect(Urho3D::SharedPtr<IoOutputSlot> out)
{
	assert(out.NotNull());

	for (int i = 0; i < (int)out->linkedInputSlots_.Size(); ++i) {
		Urho3D::SharedPtr<IoInputSlot> in = out->linkedInputSlots_[i];

		if (in.NotNull()) {
			assert(in->linkedOutputSlot_ == out);
			assert(out->linkedInputSlots_[i] == in);

			out->linkedInputSlots_[i].Reset(); // do not erase yet, it invalids index positions

			in->linkedOutputSlot_.Reset();
			in->Lose();
		}
	}

	out->linkedInputSlots_.Clear();
}

// searches for an ESTABLISHED connection at in; if found, breaks the connection
void mDisconnect(Urho3D::SharedPtr<IoInputSlot> in)
{
	assert(in.NotNull());
	Urho3D::SharedPtr<IoOutputSlot> out = in->linkedOutputSlot_;

	if (out.NotNull()) {

		int index = -1;
		for (int i = 0; i < (int)out->linkedInputSlots_.Size(); ++i) {
			if (out->linkedInputSlots_[i] == in) {
				index = i;
				break;
			}
		}

		if (index != -1) {
			assert(IndexInRange(index, (int)out->linkedInputSlots_.Size()));
			assert(out->linkedInputSlots_[index] == in);

			mDisconnect(out, index, in);
		}
	}
}

// breaks an ESTABLISHED connection between in & out; indexIntoOut required, as this connection is expected to be valid
// invalidates previous index data for out's linkedInputSlots_
void mDisconnect(
	Urho3D::SharedPtr<IoOutputSlot> out,
	int indexIntoOut,
	Urho3D::SharedPtr<IoInputSlot> in
)
{
	assert(out.NotNull());
	assert(in.NotNull());

	assert(IndexInRange(indexIntoOut, (int)out->linkedInputSlots_.Size()));
	assert(out->linkedInputSlots_[indexIntoOut] == in);
	assert(in->linkedOutputSlot_ == out);

	out->linkedInputSlots_[indexIntoOut].Reset();
	out->linkedInputSlots_.Erase(indexIntoOut);

	in->linkedOutputSlot_.Reset();
	in->Lose();
}
