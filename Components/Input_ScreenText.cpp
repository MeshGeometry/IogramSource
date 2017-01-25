#include <Urho3D/UI/Slider.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/UI.h>

#include "Input_ScreenText.h"
#include "Widget_OptionSlider.h"

#include <Urho3D/Resource/ResourceCache.h>
#include "IoGraph.h"

using namespace Urho3D;

String Input_ScreenText::iconTexture = "Textures/Icons/Input_ScreenText.png";

Input_ScreenText::Input_ScreenText(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("ScreenText");
	SetFullName("Screen Text");
	SetDescription("Adds some text to the screen UI");

	AddInputSlot(
		"Position",
		"P",
		"Optional position. If left blank, the element is automatically positioned.",
		VAR_VECTOR3,
		ITEM,
		Variant()
	);

	AddInputSlot(
		"Text",
		"T",
		"Text to display",
		VAR_STRING,
		ITEM,
		"My Text"
	);

	AddInputSlot(
		"Font",
		"F",
		"Path to Font to use",
		VAR_STRING,
		ITEM,
		"Fonts/lato-regular.ttf"
	);

	AddInputSlot(
		"Size",
		"S",
		"Font Size",
		VAR_INT,
		ITEM,
		18
	);

	AddInputSlot(
		"Priority",
		"I",
		"Priority",
		VAR_INT,
		ITEM,
		0
	);

	AddInputSlot(
		"Parent",
		"PE",
		"Optional Parent element",
		VAR_PTR,
		ITEM
	);

	AddOutputSlot(
		"TextElement",
		"TE",
		"Text Element",
		VAR_PTR,
		ITEM
	);


}

void Input_ScreenText::PreLocalSolve()
{
	//get the ui subsystem
	UI* ui = GetSubsystem<UI>();

	//delete all existing ui elements
	for (int i = 0; i < trackedItems.Size(); i++)
	{
		trackedItems[i]->SetEnabled(false);
		trackedItems[i]->Remove();
	}

	trackedItems.Clear();
}

void Input_ScreenText::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	//get the ui subsystem
	UI* ui = GetSubsystem<UI>();

	bool useCoords = false;
	if (inSolveInstance[0].GetType() == VAR_VECTOR3)
	{
		useCoords = true;
	}

	String textToDisplay = inSolveInstance[1].ToString();
	String fontPath = inSolveInstance[2].GetString();
	int fontSize = inSolveInstance[3].GetInt();
	int idx = inSolveInstance[4].GetInt();
	UIElement* parent = (UIElement*)inSolveInstance[5].GetPtr();

	//get the active ui region
	UIElement* root = (UIElement*)GetGlobalVar("activeUIRegion").GetPtr();
	if (!root)
	{
		SetAllOutputsNull(outSolveInstance);
	}

	//attach
	UIElement* container = (parent == NULL) ? root : parent;

	Text* textElement = container->CreateChild<Text>("CustomUI", idx);

	if (useCoords)
	{
		Vector3 coords = inSolveInstance[0].GetVector3();
		textElement->SetPosition(coords.x_, coords.y_);
	}

	textElement->SetStyle("LabelText");

	textElement->SetFontSize(fontSize);

	Font* font = GetSubsystem<ResourceCache>()->GetResource<Font>(fontPath);
	if (font)
	{
		textElement->SetFont(font);
	}

	textElement->SetText(textToDisplay);

	trackedItems.Push(textElement);

	outSolveInstance[0] = (Text*)textElement;
}
