#pragma once

#include "IoComponentBase.h"

class URHO3D_API Interop_SystemCommand : public IoComponentBase {

	URHO3D_OBJECT(Interop_SystemCommand, IoComponentBase)

public:
	Interop_SystemCommand(Urho3D::Context* context);
public:

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	static Urho3D::String iconTexture;
};