#pragma once

#include "IoComponentBase.h"

class URHO3D_API Interop_AsyncSystemCommand : public IoComponentBase {

	URHO3D_OBJECT(Interop_AsyncSystemCommand, IoComponentBase)

public:
	Interop_AsyncSystemCommand(Urho3D::Context* context);
public:

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	static Urho3D::String iconTexture;
};
