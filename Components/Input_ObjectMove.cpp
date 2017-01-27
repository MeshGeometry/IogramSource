#include "Input_ObjectMove.h"

#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/Input/Input.h>
#include "IoGraph.h"

#include "Urho3D/Physics/PhysicsWorld.h"
#include "Urho3D/Physics/RigidBody.h"

using namespace Urho3D;

String Input_ObjectMove::iconTexture = "Textures/Icons/Input_ObjectMove.png";

Input_ObjectMove::Input_ObjectMove(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("ObjectMove");
	SetFullName("ObjectMove");
	SetDescription("Moves an object based on user interaction");

	AddInputSlot(
		"Constraints",
		"C",
		"Movement constraints. One bit for each of the 6 degrees of freedom",
		VAR_INT,
		ITEM,
		7
	);

	AddInputSlot(
		"Local Coords",
		"LC",
		"Using local Coords for translation",
		VAR_BOOL,
		ITEM,
		false
	);

	AddInputSlot(
		"Node Filter",
		"F",
		"Only allow movement on these nodes. If 0, all nodes are allowed to move.",
		VAR_INT,
		LIST,
		0
	);

	AddOutputSlot(
		"ID",
		"ID",
		"ID of affected node",
		VAR_INT,
		ITEM
	);

	AddOutputSlot(
		"Transform",
		"T",
		"Transform defining movement",
		VAR_MATRIX3X4,
		ITEM
	);

	SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(Input_ObjectMove, HandleMouseDown));
	//SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(Input_ObjectMove, HandleMouseMove));
	SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(Input_ObjectMove, HandleMouseUp));

}


void Input_ObjectMove::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	constraintFlags = inSolveInstance[0].GetInt();

	if (!IsSolveEnabled())
	{
		UnsubscribeFromEvent(E_MOUSEBUTTONDOWN);
		UnsubscribeFromEvent(E_MOUSEBUTTONUP);
	}
	else
	{
		SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(Input_ObjectMove, HandleMouseDown));
		SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(Input_ObjectMove, HandleMouseUp));
	}

	objectFilter.Clear();
	VariantVector filterList = inSolveInstance[2].GetVariantVector();
	for (int i = 0; i < filterList.Size(); i++)
	{
		objectFilter.Push(filterList[i].GetInt());
	}
	
	//TODO: compute transform from movement
	outSolveInstance[0] = nodeID;
}

void Input_ObjectMove::HandleMouseMove(StringHash eventType, VariantMap& eventData)
{
	using namespace MouseMove;

	if (eventData[P_BUTTONS].GetInt() != MOUSEB_LEFT)
		return;
	

	int dx = eventData[P_DX].GetInt();
	int dy = eventData[P_DY].GetInt();

	int x = eventData[P_X].GetInt();
	int y = eventData[P_Y].GetInt();

	//use delta and object transform to move object
	Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();
	if (!currentCamera || !currentNode || !activeViewport)
	{
		return;
	}

	

	Graphics* graphics = GetSubsystem<Graphics>();

	Vector3 camToNode = orgHitPoint - currentCamera->GetNode()->GetWorldPosition();
	Vector3 newPos = activeViewport->ScreenToWorldPoint(x, y, camToNode.Length());

	//remap mouse pos by ui rect
	UIElement* element = (UIElement*)GetGlobalVar("activeUIRegion").GetPtr();
	if (element)
	{
		IntVector2 ePos = element->GetScreenPosition();
		IntVector2 eSize = element->GetSize();
		float sx = (x - ePos.x_) / (float)eSize.x_;
		float sy = (y - ePos.y_) / (float)eSize.y_;

		newPos = currentCamera->ScreenToWorldPoint(Vector3(sx, sy, camToNode.Length()));
	}


	//URHO3D_LOGINFO("old pos: " + String(currentNode->GetWorldPosition()));
	//URHO3D_LOGINFO("new pos: " + String(newPos));
	//URHO3D_LOGINFO("delta: " + String(newPos - currentNode->GetWorldPosition()));

	//Vector3 currPos = currentNode->GetWorldPosition();

	Vector3 moveVec = newPos - orgHitPoint;
	moveVec = GetConstrainedVector(moveVec, constraintFlags);

	RigidBody* rb = currentNode->GetComponent<RigidBody>();
	if (rb)
	{
		rb->SetKinematic(true);
		//rb->SetPosition(orgPos + (newPos - orgHitPoint));
		rb->ApplyWorldTransform(orgPos + moveVec, currentNode->GetRotation());
	}
	else
	{
		currentNode->SetWorldPosition(orgPos + moveVec);
	}


	solvedFlag_ = 0;
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();

}

void Input_ObjectMove::HandleMouseDown(StringHash eventType, VariantMap& eventData)
{
	using namespace MouseButtonDown;

	if (eventData[P_BUTTON].GetInt() == MOUSEB_LEFT)
	{
		//raycast
		if (DoRaycast())
		{
			//check if object under raycast is int the filter
			if (objectFilter.Contains(nodeID))
			{

				SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(Input_ObjectMove, HandleMouseMove));
			}

		}
	}
}



void Input_ObjectMove::HandleMouseUp(StringHash eventType, VariantMap& eventData)
{
	if (currentNode)
	{
		RigidBody* rb = currentNode->GetComponent<RigidBody>();
		if (rb)
		{
			rb->SetKinematic(false);
		}

		//not sure
		currentNode = NULL;
		currentCamera = NULL;

		solvedFlag_ = 0;
		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();

	}

	
	UnsubscribeFromEvent(E_MOUSEMOVE);
}

bool Input_ObjectMove::DoRaycast()
{
	currentNode = NULL;
	currentCamera = NULL;

	Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (!activeViewport || !scene)
	{
		return false;
	}

	UI* ui = GetSubsystem<UI>();
	IntVector2 pos = ui->GetCursorPosition();
	Graphics* graphics = GetSubsystem<Graphics>();
	currentCamera = activeViewport->GetCamera();

	//remap mouse pos by ui rect
	UIElement* element = (UIElement*)GetGlobalVar("activeUIRegion").GetPtr();

	Ray cameraRay = activeViewport->GetScreenRay(pos.x_, pos.y_);
	
	if (element)
	{
		IntVector2 ePos = element->GetScreenPosition();
		IntVector2 eSize = element->GetSize();
		float x = (pos.x_ - ePos.x_) / (float)eSize.x_;
		float y = (pos.y_ - ePos.y_) / (float)eSize.y_;

		cameraRay = currentCamera->GetScreenRay(x, y);
	}



	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, 1000.0f,
		DRAWABLE_GEOMETRY);

	
	scene->GetComponent<Octree>()->RaycastSingle(query);

	if (results.Size() > 0)
	{
		currentNode = results.At(0).drawable_->GetNode();
		orgPos = currentNode->GetWorldPosition();
		orgRot = currentNode->GetWorldRotation();
		orgHitPoint = results.At(0).position_;
		nodeID = currentNode->GetID();
		return true;
	}

	return false;
	
}

Vector3 Input_ObjectMove::GetConstrainedVector(Vector3 moveVec, int flags)
{
	Vector3 projVec(0, 0, 0);

	if (flags & 1)
	{
		Vector3 axis = Vector3::RIGHT;
		float scale = moveVec.ProjectOntoAxis(axis);
		projVec += scale * axis;
	}

	if (flags & 2)
	{
		Vector3 axis = Vector3::UP;
		float scale = moveVec.ProjectOntoAxis(axis);
		projVec += scale * axis;
	}

	if (flags & 4)
	{
		Vector3 axis = Vector3::FORWARD;
		float scale = moveVec.ProjectOntoAxis(axis);
		projVec += scale * axis;
	}

	return projVec;
}