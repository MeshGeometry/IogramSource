#include "Input_EditGeometryListener.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

#include "IoGraph.h"

using namespace Urho3D;

String Input_EditGeometryListener::iconTexture = "Textures/Icons/Input_EditGeometryListener.png";


Input_EditGeometryListener::Input_EditGeometryListener(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("EditGeometryListener");
	SetFullName("EditGeometryListener");
	SetDescription("Listens for updates to editable geometry");

	AddInputSlot(
		"NodeID",
		"ID",
		"Id of edit geometry to listen to.",
		VAR_INT,
		ITEM
		);

	AddOutputSlot(
		"Geometry",
		"G",
		"Edited geometry",
		VAR_VARIANTMAP,
		ITEM
		);

	//AddOutputSlot(
	//	"Displacements",
	//	"D",
	//	"Displacement vectors of original geometry to new.",
	//	VAR_VARIANTMAP,
	//	ITEM
	//	);

	SubscribeToEvent("EditGeometryUpdate", URHO3D_HANDLER(Input_EditGeometryListener, HandleEditGeometry));

}

void Input_EditGeometryListener::PreLocalSolve()
{
	//UnsubscribeFromAllEvents();
}

void Input_EditGeometryListener::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	//int nodeID = inSolveInstance[0].GetInt();

	//Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	//if (scene)
	//{
	//	Node* n = scene->GetChild(nodeID);
	//	if (n)
	//	{
	//		
	//	}
	//}

	outSolveInstance[0] = currentGeometry;
}

void Input_EditGeometryListener::HandleEditGeometry(StringHash eventType, VariantMap& eventData)
{
	Node* n = (Node*)eventData["NodeReference"].GetPtr();
	if (n)
	{
		currentGeometry = n->GetVar("ReferenceGeometry");
		solvedFlag_ = 0;
		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}
}