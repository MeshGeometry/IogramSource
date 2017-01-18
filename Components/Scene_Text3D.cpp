#include "Scene_Text3D.h"
#include <Urho3D/UI/Text3D.h>
#include <Urho3D/UI/Font.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Scene_Text3D::iconTexture = "Textures/Icons/Scene_Text3D.png";

Scene_Text3D::Scene_Text3D(Context* context) : IoComponentBase(context, 0, 0)
{
	//set up component info
	SetName("Text3D");
	SetFullName("Text3D");
	SetDescription("Creates a 3D text node at given position and scale");

	AddInputSlot(
		"Text",
		"T",
		"Text to display",
		VAR_STRING,
		DataAccess::ITEM,
		""
	);

	AddInputSlot(
		"Positon",
		"P",
		"Position of text node.",
		VAR_VECTOR3,
		DataAccess::ITEM,
		Vector3(0,0,0)
	);

	AddInputSlot(
		"Font",
		"F",
		"Font to display",
		VAR_STRING,
		DataAccess::ITEM,
		"Fonts/Anonymous Pro.sdf"
	);

	AddInputSlot(
		"Scale",
		"S",
		"Scale of text",
		VAR_FLOAT,
		DataAccess::ITEM,
		1.0f
	);

	AddInputSlot(
		"Options",
		"O",
		"3D Text options as bitmask. 1 - face camera, 2 - keep vertical, 4 - constant pixel size.",
		VAR_INT,
		DataAccess::ITEM,
		1
	);

	AddOutputSlot(
		"ID",
		"Node ID",
		"ID of node",
		VAR_INT,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"TextPointer",
		"T",
		"Pointer to text component",
		VAR_PTR,
		DataAccess::ITEM
	);
}

void Scene_Text3D::PreLocalSolve()
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedItems.Size(); i++)
		{
			Node* n = scene->GetNode(trackedItems[i]);
			if (n != NULL)
			{
				n->Remove();
			}
		}
	}

	trackedItems.Clear();
}

//work function
void Scene_Text3D::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered!");
		outSolveInstance[0] = Variant();
		return;
	}

	//otherwise proceed with finding the node
	String displayText = inSolveInstance[0].ToString();
	Vector3 pos = inSolveInstance[1].GetVector3();
	String fontName = inSolveInstance[2].GetString();
	float scale = inSolveInstance[3].GetFloat();
	int options = inSolveInstance[4].GetInt();

	Node* node = scene->CreateChild("3DText");
	Text3D* text = node->CreateComponent<Text3D>();
	Font* f = GetSubsystem<ResourceCache>()->GetResource<Font>(fontName);
	if (f)
		text->SetFont(f);
	text->SetText(displayText);
	node->SetPosition(pos);
	node->SetScale(Vector3(scale, scale, scale));
	text->SetAlignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER);

	text->SetAttribute("Face Camera Mode", options);
	trackedItems.Push(text->GetID());
	
	outSolveInstance[0] = node->GetID();
	outSolveInstance[1] = text;
}