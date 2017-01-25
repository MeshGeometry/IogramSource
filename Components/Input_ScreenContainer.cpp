#include "Input_ScreenContainer.h"
#include "Widget_Container.h"

#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Slider.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "IoGraph.h"

using namespace Urho3D;

String Input_ScreenContainer::iconTexture = "Textures/Icons/Input_ScreenContainer.png";

Input_ScreenContainer::Input_ScreenContainer(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("ScreenContainer");
	SetFullName("Screen Container");
	SetDescription("A container for UI objects.");

	AddInputSlot(
		"Name",
		"N",
		"Name of this container",
		VAR_STRING,
		ITEM,
		"My Group"
	);

	AddInputSlot(
		"Position",
		"P",
		"Optional position. If left blank, the element is automatically positioned.",
		VAR_VECTOR3,
		ITEM,
		Vector3(10, 50, 0)
	);

	AddInputSlot(
		"Size",
		"S",
		"Optional size. If left blank, the element is automatically size.",
		VAR_VECTOR3,
		ITEM,
		Vector3(320, 200, 0)
	);

	AddInputSlot(
		"Options",
		"O",
		"Container options",
		VAR_INT,
		ITEM,
		1
	);

	AddOutputSlot(
		"ContainerElement",
		"C",
		"Container Element",
		VAR_PTR,
		ITEM
	);

}

void Input_ScreenContainer::PreLocalSolve()
{
	//get the ui subsystem
	UI* ui = GetSubsystem<UI>();

	//delete all existing ui elements
	for (int i = 0; i < trackedItems.Size(); i++)
	{	
		//move all added elements to root
		PODVector<UIElement*> addedElements;
		trackedItems[i]->GetContentElement()->GetChildren(addedElements, false);
		for (int i = 0; i < addedElements.Size(); i++)
		{
			addedElements[i]->SetParent(ui->GetRoot());
		}
		
		trackedItems[i]->Remove();
	}

	trackedItems.Clear();
}

void Input_ScreenContainer::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	//get the ui subsystem
	UI* ui = GetSubsystem<UI>();




	
	String name = inSolveInstance[0].ToString();
	Vector3 coords = inSolveInstance[1].GetVector3();
	Vector3 size = inSolveInstance[2].GetVector3();
	int options = inSolveInstance[3].GetInt();

	//get the active ui region
	UIElement* root = (UIElement*) GetGlobalVar("activeUIRegion").GetPtr();
	if (!root)
	{
		SetAllOutputsNull(outSolveInstance);
	}

	Widget_Container* container = root->CreateChild<Widget_Container>("CustomContainer");
	XMLFile* styleFile = GetSubsystem<ResourceCache>()->GetResource<XMLFile>("UI/IogramDefaultStyle.xml");
	container->SetStyle("Widget_Container", styleFile);
	container->CustomInterface();
	container->SetPosition(coords.x_, coords.y_);
	container->SetSize(size.x_, size.y_);
	container->GetTextElement()->SetText(name);

	//set some options
	if (options & 1)
	{
		container->isMovable = true;
	}

	trackedItems.Push(container);

	outSolveInstance[0] = container->GetContentElement();
}
