#pragma once

#include <Urho3D/Container/Ptr.h>

#include "IoInputSlot.h"
#include "IoOutputSlot.h"

class IoInputSlot;
class IoOutputSlot;

void mConnect(
	Urho3D::SharedPtr<IoOutputSlot> out,
	Urho3D::SharedPtr<IoInputSlot> in
);

void mDisconnect(Urho3D::SharedPtr<IoInputSlot> in);

void mDisconnect(Urho3D::SharedPtr<IoOutputSlot> out);

void mDisconnect(
	Urho3D::SharedPtr<IoOutputSlot> out,
	int indexIntoOut,
	Urho3D::SharedPtr<IoInputSlot> in
);