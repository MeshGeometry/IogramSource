#pragma once

#include "IoComponentBase.h"
#include <Urho3D/Graphics/BillboardSet.h>

class URHO3D_API Scene_Display : public IoComponentBase {
	URHO3D_OBJECT(Scene_Display, IoComponentBase)
public:
	Scene_Display(Urho3D::Context* context);

	virtual void PreLocalSolve();

	Urho3D::Vector<int> trackedItems;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;

	Urho3D::BillboardSet* pointCloud;
    

	///normal preview material
#ifdef EMSCRIPTEN
	Urho3D::String normalMat = "Materials/BasicWeb.xml";
	Urho3D::String normalAlphaMat = "Materials/BasicWebAlpha.xml";
#endif

#ifndef EMSCRIPTEN
	Urho3D::String normalMat = "Materials/BasicPBR.xml";
	Urho3D::String normalAlphaMat = "Materials/BasicPBRAlpha.xml";
#endif
	
	Urho3D::String normalMatWires = "Materials/BasicWireframe.xml";
	Urho3D::String pointMat = "Materials/BasicPoints.xml";
	Urho3D::String widget = "Materials/BasicTransparent.xml";
};
