//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "Scene_SelectGeometry.h"
#include "Urho3D/UI/Button.h"
#include "Urho3D/UI/Text.h"
#include "Urho3D/Graphics/BillboardSet.h"
#include "IoGraph.h"
#include "TriMesh.h"
#include <iostream>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Viewport.h>

using namespace Urho3D;

String Scene_SelectGeometry::iconTexture = "Textures/Icons/Scene_SelectGeometry.png";

Scene_SelectGeometry::Scene_SelectGeometry(Urho3D::Context* context) : IoComponentBase(context, 1, 4)
{
	SetName("SelectGeometry");
	SetFullName("Select Geometry");
	SetDescription("Returns selected geometry");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("");

	inputSlots_[0]->SetName("On");
	inputSlots_[0]->SetVariableName("On");
	inputSlots_[0]->SetDescription("Activate selection");
	inputSlots_[0]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(true);
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("MeshesOut");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Meshes out");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[1]->SetName("PolylinesOut");
	outputSlots_[1]->SetVariableName("P");
	outputSlots_[1]->SetDescription("Polylines out");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[1]->SetDataAccess(DataAccess::LIST);

	outputSlots_[2]->SetName("PointsOut");
	outputSlots_[2]->SetVariableName("Pt");
	outputSlots_[2]->SetDescription("Points out");
	outputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[2]->SetDataAccess(DataAccess::LIST);

	outputSlots_[3]->SetName("Mouse Position");
	outputSlots_[3]->SetVariableName("MP");
	outputSlots_[3]->SetDescription("Mouse Position");
	outputSlots_[3]->SetVariantType(VariantType::VAR_VECTOR3); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[3]->SetDataAccess(DataAccess::ITEM);
}

void Scene_SelectGeometry::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	VariantVector meshesOut;
	VariantVector polylinesOut;
	VariantVector pointsOut;

	//////////////////////////
	// Mouse Click Listener //
	//////////////////////////
	//subscribe or unsuscribe
	bool currFlag = inSolveInstance[0].GetBool();
	if (currFlag && !isOn)
	{
		SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(Scene_SelectGeometry, HandleMouseClick));
		isOn = true;
	}
	if (!currFlag && isOn)
	{
		UnsubscribeFromEvent(E_MOUSEBUTTONDOWN);
		isOn = false;
	}

	outSolveInstance[3] = mPos;

	////////////////////////
	// Screen Cast to Ray //
	////////////////////////
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	Camera* cam = scene->GetComponent<Camera>(true);
	Graphics* graphics = GetSubsystem<Graphics>();
		
	//Vector3 sp = inSolveInstance[0].GetVector3();
	Vector3 sp = mPos;

	Ray sr = cam->GetScreenRay(sp.x_ / graphics->GetWidth(), sp.y_ / graphics->GetHeight());

	//handle active viewport case
	Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();
	if (activeViewport)
	{
		IntRect viewRect = activeViewport->GetRect();
		float width = viewRect.Width();
		float height = viewRect.Height();
		float x = (float)(sp.x_ - viewRect.left_) / width;
		float y = (float)(sp.y_ - viewRect.top_) / height;

		sr = cam->GetScreenRay(x, y);
	}

	//////////////
	// Ray Cast //
	//////////////

	// TODO: what should the maxD actually be?
	float maxD = 100000.0f;

	Ray ray = Ray(sr.origin_, sr.direction_);
	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, ray, RAY_TRIANGLE, maxD, DRAWABLE_GEOMETRY);

	//HACK! turn off any billboards in the scene
	PODVector<BillboardSet*> comps;
	scene->GetComponents<BillboardSet>(comps, true);

	for (int i = 0; i < comps.Size(); i++)
		comps[i]->SetEnabled(false);

	scene->GetComponent<Octree>()->RaycastSingle(query);

	if (results.Size() > 0 && inSolveInstance[0].GetBool() == true)
	{
		VariantVector verts;
		VariantVector faces;
		VariantVector normals;

		Urho3D::Drawable* curr_draw = results[0].drawable_;
		
		// Checking whether the new drawable is already on the list. If it is, remove it 
		Vector<Drawable*>::Iterator draw_iter = drawables.Find(curr_draw);
		if (draw_iter == drawables.End())
			drawables.Push(curr_draw);
		else {
			drawables.Remove(curr_draw);
		}

		// Now iterate over the drawables and parse.
		for (int i = 0; i < drawables.Size(); ++i)
		{
			Urho3D::Drawable* draw = drawables[i];

			// Parse the vertices and normals
			Geometry* geo = draw->GetLodGeometry(0, 0);
			Urho3D::PrimitiveType primitive_type = geo->GetPrimitiveType();
			VertexBuffer* vb = geo->GetVertexBuffer(0);

			if (primitive_type == PrimitiveType::TRIANGLE_LIST) {

				const unsigned char* vertexData = (const unsigned char*)vb->Lock(0, vb->GetVertexCount());
				if (vertexData) {
					unsigned numVertices = vb->GetVertexCount();
					unsigned vertexSize = vb->GetVertexSize();
					// Copy the original vertex positions
					for (unsigned i = 0; i < numVertices; ++i)
					{
						unsigned numVertices = vb->GetVertexCount();
						unsigned vertexSize = vb->GetVertexSize();
						// Copy the original vertex positions
						for (unsigned i = 0; i < numVertices; ++i)
						{
							const Vector3& src = *reinterpret_cast<const Vector3*>(vertexData + i * vertexSize);
							const Vector3& norm = *reinterpret_cast<const Vector3*>(vertexData + i * vertexSize + 12);
							verts.Push(src);
							normals.Push(norm);
						}
						vb->Unlock();
					}
				}

				// Parse the faces
				IndexBuffer* ib = geo->GetIndexBuffer();
				int* indexData = (int*)ib->Lock(0, ib->GetIndexCount());
				if (indexData)
				{
					unsigned numIndices = ib->GetIndexCount();
					unsigned indexSize = ib->GetIndexSize();

					// Copy the original vertex positions
					for (unsigned i = 0; i < numIndices; ++i)
					{
						int a = indexData[i];
						faces.Push(a);
					}
					ib->Unlock();

					Variant outMesh = TriMesh_Make(verts, faces);
					meshesOut.Push(outMesh);
				}
			}
			else if (primitive_type == PrimitiveType::LINE_LIST) {
				// deal with Grid here?
			}
			// Other PrimitiveTypes here? E.g. POINT_LIST
		}

		outSolveInstance[0] = meshesOut;

	}

	//turn points back on
	for (int i = 0; i < comps.Size(); i++)
		comps[i]->SetEnabled(true);
}

void Scene_SelectGeometry::HandleMouseClick(StringHash eventType, VariantMap& eventData)
{
	using namespace MouseButtonDown;

	IntVector2 pos = GetSubsystem<Input>()->GetMousePosition();
	mPos = Vector3(pos.x_, pos.y_, 0);

	Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();
	if (activeViewport)
	{
		IntRect viewRect = activeViewport->GetRect();
		mPos = Vector3(pos.x_ - viewRect.left_, pos.y_ - viewRect.top_, 0);
	}

	solvedFlag_ = 0;
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}