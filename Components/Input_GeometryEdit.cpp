#include "Input_GeometryEdit.h"

#include <Urho3D/Resource/ResourceCache.h>
#include "IoGraph.h"

#include "Urho3D/Physics/PhysicsWorld.h"
#include "Urho3D/Physics/RigidBody.h"
#include "Urho3D/Scene/Node.h"
#include "Urho3D/Graphics/Camera.h"
#include "Urho3D/Input/InputEvents.h"
#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/UI/UI.h>

#include "Polyline.h"
#include "TriMesh.h"

using namespace Urho3D;

String Input_GeometryEdit::iconTexture = "Textures/Icons/Input_GeometryEdit.png";

Input_GeometryEdit::Input_GeometryEdit(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("GeometryEdit");
	SetFullName("GeometryEdit");
	SetDescription("Allows the user to manipulate geometry vertices.");

	AddInputSlot(
		"Geometry",
		"G",
		"Geometry to operate on.",
		VAR_VARIANTMAP,
		ITEM
		);

	AddInputSlot(
		"Constraints",
		"C",
		"Movement constraints. One bit for each of the 6 degrees of freedom",
		VAR_INT,
		ITEM,
		7
		);

	AddInputSlot(
		"DisplayThickness",
		"T",
		"Thickness of display curves",
		VAR_FLOAT,
		ITEM,
		0.01f
	);

	AddInputSlot(
		"Color",
		"CL",
		"Color of diplay curves",
		VAR_COLOR,
		ITEM,
		Color(0.7, 0.7, 0.7)
	);


	AddOutputSlot(
		"Edit Geometry",
		"ID",
		"Reference to Edit Geometry. Connect to Edit Geometry Listener.",
		VAR_INT,
		ITEM
		);

	SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(Input_GeometryEdit, HandleMouseDown));
	//SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(Input_GeometryEdit, HandleMouseMove));
	SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(Input_GeometryEdit, HandleMouseUp));

}

void Input_GeometryEdit::PreLocalSolve()
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedItems.Size(); i++)
		{
			Node* n = scene->GetNode(trackedItems[i]);
			if (n)
			{
				n->Remove();
			}
		}
	}

	trackedItems.Clear();
}


void Input_GeometryEdit::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	constraintFlags = inSolveInstance[1].GetInt();


	/*
	THE IDEA:

	1) Create a Billboard set in the style of the line renderer for each geometry object
	2) Listen for mouse left mouse clicks and do a ray cast agains these billboard sets.
	3) if the ray cast is good, move the individual billboard by the given move vector
	4) update the input geometry and call solve with this solved flag as 1.

	Q: how to link billboards to vertices?
	A: The billboard set ray query result will provide the sub object index (i.e. the billboard). That can be used to recover the vertex id

	*/


	if (!IsSolveEnabled())
	{
		UnsubscribeFromEvent(E_MOUSEBUTTONDOWN);
		UnsubscribeFromEvent(E_MOUSEBUTTONUP);
	}
	else
	{
		SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(Input_GeometryEdit, HandleMouseDown));
		SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(Input_GeometryEdit, HandleMouseUp));
	}

	//create the renderer
	float thickness = inSolveInstance[2].GetFloat();
	Color col = inSolveInstance[3].GetColor();
	int editGeom = CreateEditGeometry(inSolveInstance[0], thickness, col);
	trackedItems.Push(editGeom);
	outSolveInstance[0] = editGeom;


}

void Input_GeometryEdit::HandleMouseMove(StringHash eventType, VariantMap& eventData)
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
	Camera* currentCamera = activeViewport->GetCamera();
	if (!currentCamera || !activeViewport)
	{
		return;
	}



	Graphics* graphics = GetSubsystem<Graphics>();


	//test move
	BillboardSet* bs = (BillboardSet*)currentHitResult.drawable_;
	BillboardSet* vGeom = (BillboardSet*)currentHitResult.node_->GetVar("VertexGeometry").GetPtr();
	if (bs && bs == vGeom)
	{
		//retrieve original vertex position
		Variant geom = currentHitResult.node_->GetVar("ReferenceGeometry");
		VariantMap geomMap = geom.GetVariantMap();
		VariantVector verts = geomMap["vertices"].GetVariantVector();

		//get the specific billboard
		int bIndex = currentHitResult.subObject_;
		Billboard* b = bs->GetBillboard(bIndex);

		Vector3 orgHitPoint = verts[bIndex].GetVector3(); //currentHitResult.position_;
		Vector3 camToNode = currentHitResult.position_ - currentCamera->GetNode()->GetWorldPosition();
		Vector3 newPos = activeViewport->ScreenToWorldPoint(x, y, camToNode.Length());

		Vector3 moveVec = newPos - orgHitPoint;
		moveVec = GetConstrainedVector(moveVec, constraintFlags);

		if (bIndex >= verts.Size())
		{
			return;
		}

		//move the vertex
		Vector3 currVert = verts[bIndex].GetVector3();

		//update any connected edges
		BillboardSet* edgeSet = (BillboardSet*)currentHitResult.node_->GetVar("EdgeGeometry").GetPtr();
		if (edgeSet)
		{
			int numEdges = edgeSet->GetNumBillboards();
			for (int i = 0; i < numEdges; i++)
			{
				//calculate end points from billboard dims
				Billboard* eb = edgeSet->GetBillboard(i);
				Vector3 midPt = eb->position_;
				Vector2 size = eb->size_;
				Vector3 startPt = midPt - 0.5f * eb->direction_;
				Vector3 endPt = midPt + 0.5f * eb->direction_;

				//check if start or end pt is close to moved edit point
				Vector3 targetPt = currVert + moveVec;
				if ((startPt - b->position_).Length() < 0.0001f)
				{
					eb->position_ = targetPt + 0.5f * (endPt - targetPt);
					eb->size_ = Vector2(size.x_, 0.5f * ((endPt - targetPt).Length()) - 4 * size.x_);
					eb->direction_ = endPt - targetPt;
				}

				if ((endPt - b->position_).Length() < 0.0001f)
				{
					eb->position_ = startPt + 0.5f * (targetPt - startPt);
					eb->size_ = Vector2(size.x_, 0.5f * ((targetPt - startPt).Length()) - 4 * size.x_);
					eb->direction_ = targetPt - startPt;
				}
			}

			edgeSet->Commit();
		}

		//move the vertex
		b->position_ = currVert + moveVec;
		bs->Commit();

		//update the reference geometry
		verts[bIndex] = currVert + moveVec;
		geomMap["vertices"] = verts;
		currentHitResult.node_->SetVar("ReferenceGeometry", geomMap);

		VariantMap data;
		data["NodeReference"] = currentHitResult.node_;
		currentHitResult.node_->SendEvent("EditGeometryUpdate", data);
		//update output and call solve
		//solvedFlag_ = 0;
		//GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}




}

void Input_GeometryEdit::HandleMouseDown(StringHash eventType, VariantMap& eventData)
{
	using namespace MouseButtonDown;


	if (eventData[P_BUTTON].GetInt() == MOUSEB_LEFT)
	{
		//raycast
		if (DoRaycast())
		{

			//check if object under raycast is int the filter
			if (trackedItems.Contains(currentHitResult.node_->GetID()))
			{
				SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(Input_GeometryEdit, HandleMouseMove));
			}


		}
	}
}


void Input_GeometryEdit::HandleMouseUp(StringHash eventType, VariantMap& eventData)
{
	UnsubscribeFromEvent(E_MOUSEMOVE);
}

bool Input_GeometryEdit::DoRaycast()
{

	Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (!activeViewport || !scene)
	{
		return false;
	}

	UI* ui = GetSubsystem<UI>();
	IntVector2 pos = ui->GetCursorPosition();
	Graphics* graphics = GetSubsystem<Graphics>();
	Ray cameraRay = activeViewport->GetScreenRay(pos.x_, pos.y_);

	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, 100.0f,
		DRAWABLE_GEOMETRY);


	scene->GetComponent<Octree>()->Raycast(query);

	if (results.Size() > 0)
	{
		for (int i = 0; i < results.Size(); i++)
		{
			if (trackedItems.Contains(results.At(i).node_->GetID()))
			{
				BillboardSet* bs = (BillboardSet*)results.At(i).node_->GetVar("VertexGeometry").GetPtr();
				if (bs && bs == results.At(i).drawable_)
				{
					currentHitResult = results.At(i);
					return true;
				}

			}

		}
		

	}

	return false;

}

Vector3 Input_GeometryEdit::GetConstrainedVector(Vector3 moveVec, int flags)
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

int Input_GeometryEdit::CreateEditGeometry(Variant geometry, float thickness, Color color)
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	VariantVector verts;
	Vector<Pair<int, int>> edges;

	//handle the admissble geometry types
	if (Polyline_Verify(geometry))
	{
		VariantMap geoMap = geometry.GetVariantMap();
		verts = geoMap["vertices"].GetVariantVector();
		edges = Polyline_ComputeEdges(geometry);
	}
	else if (TriMesh_Verify(geometry))
	{
		VariantMap geoMap = geometry.GetVariantMap();
		verts = TriMesh_GetVertexList(geometry);
		edges = TriMesh_ComputeEdges(geometry);
	}

	if (verts.Size() < 2)
	{
		return -1;
	}

	Node* geomEditNode = scene->CreateChild("GeomEditNode");

	//get the material
	Material* mat = NULL;
	mat = cache->GetResource<Material>("Materials/BasicCurve.xml");

	if (!mat)
	{
		cache->GetResource<Material>("Materials/BasicWebAlpha.xml");
	}



	//create the display
	SharedPtr<Material> clonedMatA = mat->Clone();
	BillboardSet* curveDisplay = geomEditNode->CreateComponent<BillboardSet>();
	curveDisplay->SetNumBillboards(edges.Size());
	curveDisplay->SetMaterial(clonedMatA);
	curveDisplay->SetSorted(true);
	curveDisplay->SetFaceCameraMode(FaceCameraMode::FC_DIRECTION);

	SharedPtr<Material> clonedMat = mat->Clone();
	clonedMat->SetPixelShaderDefines("DRAWPOINT");
	BillboardSet* controlPointDisplay = geomEditNode->CreateComponent<BillboardSet>();
	controlPointDisplay->SetNumBillboards(verts.Size());
	controlPointDisplay->SetMaterial(clonedMat);
	controlPointDisplay->SetSorted(true);
	controlPointDisplay->SetFaceCameraMode(FaceCameraMode::FC_ROTATE_XYZ);
	//controlPointDisplay->SetFixedScreenSize(true);

	//IMPORTANT! track the geometry to edit for retrieval later
	geomEditNode->SetVar("ReferenceGeometry", geometry);

	//IMPORTANT! track the editable stuff
	geomEditNode->SetVar("VertexGeometry", controlPointDisplay);
	geomEditNode->SetVar("EdgeGeometry", curveDisplay);

	//IMPORTANT! Bind the vertex references to the edges
	VariantMap edgeVertexMap;

	float width = thickness;
	Color col = color;

	mat->SetShaderParameter("MatDiffColor", col);
	clonedMat->SetShaderParameter("MatDiffColor", col);

	//create the vertex renderer
	for (int i = 0; i < verts.Size(); i++)
	{
		Billboard* bb = controlPointDisplay->GetBillboard(i);
		Vector3 vA = verts[i].GetVector3();

		//render the point
		bb = controlPointDisplay->GetBillboard(i);
		bb->position_ = vA;
		bb->size_ = 3.0f * Vector2(width, width);
		//bb->size_ = Vector2(10, 10);
		bb->enabled_ = true;
		bb->color_ = col;

	}

	//create the edge renderer
	for (int i = 0; i < edges.Size(); i++)
	{
		//render the edge
		Billboard* bb = curveDisplay->GetBillboard(i);

		int startId = edges[i].first_;
		int endId = edges[i].second_;
		
		Vector3 vA = verts[startId].GetVector3();
		Vector3 vB = verts[endId].GetVector3();
		Vector3 edgeVec = vB - vA;
		Vector3 midPt = vA + 0.5f * edgeVec;

		bb->position_ = midPt;
		bb->size_ = Vector2(width, 0.5f * (edgeVec.Length()) - 6 * width);
		bb->direction_ = edgeVec;
		bb->enabled_ = true;
		bb->color_ = col;
	}

	curveDisplay->Commit();
	controlPointDisplay->Commit();

	return geomEditNode->GetID();
}