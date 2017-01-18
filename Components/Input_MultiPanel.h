#pragma once

#include "IoComponentBase.h"
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>

class URHO3D_API Input_MultiPanel : public IoComponentBase {

	URHO3D_OBJECT(Input_MultiPanel, IoComponentBase)

public:
	Input_MultiPanel(Urho3D::Context* context) : IoComponentBase(context, 1, 1) { };

public:

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	static Urho3D::String iconTexture;

	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);
	virtual Urho3D::String GetNodeStyle();

	//UIElement* uiRoot = NULL;
	//Button* addLine = NULL;
	//Button* deleteLine = NULL;
	//Button* addCol = NULL;
	//Button* deleteCol = NULL;
	//Vector<Vector<LineEdit*>> columns;
	//Vector<LineEdit*> lines;
	//int lineCount;

	//void HandleLineEditCommit(StringHash eventType, VariantMap& eventData);
	//void HandleNewLine(StringHash eventType, VariantMap& eventData);
	//void HandleDeleteLine(StringHash eventType, VariantMap& eventData);


	//void HandleResize(StringHash eventType, VariantMap & eventData);
};