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

#include "ModelBoxMorph.h"

#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/IO/Log.h>

#include "TriMesh.h"
#include "Mesh_BoundingBox.h"


using namespace Urho3D;

ModelBoxMorph::ModelBoxMorph(Context* context) : Component(context),
editing_(false)
{

}

void ModelBoxMorph::OnNodeSet(Urho3D::Node* node)
{
	Component::OnNodeSet(node);

	if (!node)
	{
		return;
	}
	//    //create the editor
	//    CreateMeshEditor();

	//subscribe to events
	SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(ModelBoxMorph, HandleMouseDown));
	SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(ModelBoxMorph, HandleMouseMove));
	SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(ModelBoxMorph, HandleMouseUp));
	SubscribeToEvent(E_COMPONENTREMOVED, URHO3D_HANDLER(ModelBoxMorph, HandleComponentRemoved));
}


void ModelBoxMorph::HandleComponentRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace ComponentRemoved;

	Component* c = (Component*)eventData[P_COMPONENT].GetPtr();
	if (c == this)
	{
		
		//UnsubscribeFromAllEvents();
		if (meshEditor_)
		{
			meshEditor_->Remove();
		}
	}

}

void ModelBoxMorph::CreateMeshEditor()
{
	//create billboard set

    //originalBoundingBox_= 
    boundingBox_ = TriMesh_BoundingBox(baseGeometry_);
    VariantVector verts = TriMesh_GetVertexList(boundingBox_);
	VariantVector geom_verts = TriMesh_GetVertexList(baseGeometry_);
    
    // create transformation matrix
    Vector3 v_0 = verts[0].GetVector3();
    Vector3 v_1 = verts[1].GetVector3();
    Vector3 v_2 = verts[2].GetVector3();
    Vector3 v_4 = verts[4].GetVector3();
	Vector3 b_0 = (v_1 - v_0);
	Vector3 b_1 = v_2 - v_0;
    Vector3 b_2 = v_4 - v_0;
    
    Matrix3 changeOfBasis_ = Matrix3(b_0.x_, b_1.x_, b_2.x_, b_0.y_, b_1.y_, b_2.y_, b_0.z_, b_1.z_, b_2.z_);
	Vector3 originalBoxOrigin_ = v_0;

	//sanity check: determinant of change of basis matrix is nonzero
	if ((b_0.CrossProduct(b_1).DotProduct(b_2)) == 0)
		changeOfBasis_ = Matrix3::IDENTITY;

	// first get parametrization of original vertices WRT original bounding box
	for (int i = 0; i < geom_verts.Size(); ++i) {
		Vector3 cur_v = geom_verts[i].GetVector3();
		Vector3 param_v = changeOfBasis_.Inverse()*(cur_v - originalBoxOrigin_);
		originalVertParams_.Push(param_v);
	}


	ResourceCache* rc = GetSubsystem<ResourceCache>();

	Material* mat = rc->GetResource<Material>("Materials/BasicPoints.xml");
	SharedPtr<Material> clonedMat = mat->Clone();
	//Texture* tex = rc->GetResource<Texture>("Textures/SpotWide.png");

	float width = 10.0f;
	Color col = Color::WHITE;


	//clonedMat->SetTexture(TU_DIFFUSE, tex);
	clonedMat->SetShaderParameter("MatDiffColor", col);


	meshEditor_ = GetNode()->CreateComponent<BillboardSet>();
	meshEditor_->SetNumBillboards(verts.Size());
	meshEditor_->SetMaterial(clonedMat);
	meshEditor_->SetSorted(true);
	meshEditor_->SetFaceCameraMode(FaceCameraMode::FC_ROTATE_XYZ);
	meshEditor_->SetFixedScreenSize(true);


	//create the vertex renderer
	for (int i = 0; i < verts.Size(); i++)
	{
		Billboard* bb = meshEditor_->GetBillboard(i);
		Vector3 vA = verts[i].GetVector3();

		//render the point
		bb = meshEditor_->GetBillboard(i);
		bb->position_ = vA;
		bb->size_ = Vector2(width, width);
		bb->enabled_ = true;
		bb->color_ = col;
	}

	//commit
	meshEditor_->Commit();
}



void ModelBoxMorph::SetBaseGeometry(const Urho3D::Variant geometry)
{
	baseGeometry_ = geometry;
	//create the editor
	CreateMeshEditor();

}

void ModelBoxMorph::GetCurrentGeometry(Urho3D::Variant& geometryOut)
{
	geometryOut = baseGeometry_;
}

IntVector2 ModelBoxMorph::GetScaledMousePosition()
{
	IntVector2 mPos = GetSubsystem<Input>()->GetMousePosition();
	float scale = GetSubsystem<UI>()->GetScale();
	mPos.x_ = (int)(mPos.x_ * (1.0f / scale));
	mPos.y_ = (int)(mPos.y_ * (1.0f / scale));

	return mPos;
}

void ModelBoxMorph::HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{

}

void ModelBoxMorph::HandleMouseDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace MouseButtonDown;

	int mb = eventData[P_BUTTON].GetInt();

	if (mb == MOUSEB_LEFT && !editing_) {

		if (Raycast()) {

			//UI* ui = GetSubsystem<UI>();
			startScreenPos_ = GetScaledMousePosition();
			primaryVertexID_ = raycastResult_.subObject_;
			editing_ = true;

			URHO3D_LOGINFO("Hello mouse down!");
		}
	}
}

void ModelBoxMorph::HandleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace MouseMove;

	if (editing_) {

		//mpos
		//UI* ui = GetSubsystem<UI>();
		IntVector2 mPos = GetScaledMousePosition();

		//use delta and object transform to move object
		Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();
		Camera* currentCamera = activeViewport->GetCamera();
		if (!currentCamera || !activeViewport)
		{
			return;
		}

		Ray screenRay;
		bool res = GetScreenRay(mPos, screenRay);

		if (!res)
			return;

		Vector3 sceneHint = screenRay.origin_ + raycastResult_.distance_ * screenRay.direction_;
		Vector3 sceneDeltaVec = sceneHint - raycastResult_.position_;

		URHO3D_LOGINFO("Hello mouse move!");

		//update node visuals

		//test move
		BillboardSet* bs = (BillboardSet*)raycastResult_.drawable_;
		if (bs && primaryVertexID_ < bs->GetNumBillboards())
		{
			//retrieve original vertex position

			VariantVector verts = TriMesh_GetVertexList(boundingBox_);

			//get the specific billboard
			Billboard* b = bs->GetBillboard(primaryVertexID_);


			if (primaryVertexID_ >= verts.Size())
			{
				return;
			}

			//move the vertex
			Vector3 currVert = verts[primaryVertexID_].GetVector3();

			//track displacement and index
			primaryDelta_ = sceneDeltaVec;

			//move the vertex
			b->position_ = sceneHint;
			bs->Commit();

			VariantMap data;
			data["NodeReference"] = raycastResult_.node_;
			//            data["vertID"] = bIndex;
			//            data["moveVec"] = moveVec;
			raycastResult_.node_->SendEvent("EditGeometryUpdate", data);

		}

	}
}

void ModelBoxMorph::HandleMouseUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace MouseButtonUp;

	int mb = eventData[P_BUTTON].GetInt();

	if (mb == MOUSEB_LEFT && editing_ && primaryVertexID_ >= 0) {

		editing_ = false;

		URHO3D_LOGINFO("Hello mouse up!");

		//use delta and object transform to move object
		Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();
		Camera* currentCamera = activeViewport->GetCamera();
		if (!currentCamera || !activeViewport)
		{
			return;
		}

		VariantVector boxVerts = TriMesh_GetVertexList(boundingBox_);
		VariantVector verts = TriMesh_GetVertexList(baseGeometry_);
		VariantVector faces = TriMesh_GetFaceList(baseGeometry_);

		// Need to get the billboard ID that was moved, and compute the vector from that billboard to the original vert
		// Temporarily using this:
		Vector3 disp = primaryDelta_;
		int bIndex = primaryVertexID_;

		Vector3 movedCorner = boxVerts[bIndex].GetVector3();

		//update base geometry
		boxVerts[bIndex] = movedCorner + primaryDelta_;
		boundingBox_ = TriMesh_Make(boxVerts, TriMesh_GetFaceList(boundingBox_));

		// do the box morph
		Variant geomOut;
		Vector<Vector3> morphed_verts;
		DoBoxMorph(morphed_verts, geomOut);
        
        // preserve any other keys in baseGeometry by setting verts and faces
        VariantMap baseMap = baseGeometry_.GetVariantMap();
        VariantVector rev_verts = TriMesh_GetVertexList(geomOut);
        VariantVector rev_faces = TriMesh_GetFaceList(geomOut);
        baseMap["vertices"] = Variant(rev_verts);
        baseMap["faces"] = Variant(rev_faces);
        
		baseGeometry_ = Variant(baseMap);

		//update handles based on new geometry
		UpdateHandles();

		// this is roundabout
		VariantMap data;
		data["EditedMesh"] = baseGeometry_;

		//fill data 
		SendEvent("ModelEditChangeBoxMorph", data);

		//reset
		primaryDelta_ = Vector3::ZERO;
		primaryVertexID_ = -1;

	}
}

void ModelBoxMorph::UpdateHandles()
{
	if (meshEditor_)
	{
		VariantVector verts = TriMesh_GetVertexList(boundingBox_);
		if (verts.Size() == meshEditor_->GetNumBillboards())
		{
			for (int i = 0; i < verts.Size(); i++)
			{
				meshEditor_->GetBillboard(i)->position_ = verts[i].GetVector3();
			}
		}

		meshEditor_->Commit();
	}
}

void ModelBoxMorph::DoBoxMorph(Urho3D::Vector<Urho3D::Vector3>& morphed_verts, Variant& geomOut)
{
    VariantVector boxVerts = TriMesh_GetVertexList(boundingBox_);
    VariantVector verts = TriMesh_GetVertexList(baseGeometry_);
    VariantVector faces = TriMesh_GetFaceList(baseGeometry_);

	// these should be the up to date positions of the box?
	Vector3 v0 = boxVerts[0].GetVector3();
	Vector3 v1 = boxVerts[1].GetVector3();
	Vector3 v2 = boxVerts[2].GetVector3();
	Vector3 v3 = boxVerts[3].GetVector3();
	Vector3 v4 = boxVerts[4].GetVector3();
	Vector3 v5 = boxVerts[5].GetVector3();
	Vector3 v6 = boxVerts[6].GetVector3();
	Vector3 v7 = boxVerts[7].GetVector3();
     
    for (int i = 0; i < verts.Size(); ++i)
    {
        //// first get parametrization of original vertices WRT original bounding box
        //Vector3 cur_v = verts[i].GetVector3();
        //Vector3 param_v = changeOfBasis_.Inverse()*(cur_v - originalBoxOrigin_);


		// look up original parametrization of vertices
		Vector3 param_v = originalVertParams_[i];
		float a = param_v.x_;
		float b = param_v.y_;
		float c = param_v.z_;
		        
        // compute adjusted position of vertexrelative to new box corners
		// find projection of cur_v in the YZ "planes" generated by 
		// (v0, v1, v2, v3) and (v4, v5, v6, v7)
		// then connect these projections by a line and find the appropriate x-value

		Vector3 yz_a = v0 + a*(v1 - v0) + b*(v2 + a*(v3 - v2) - v0 - a*(v1 - v0));
		Vector3 yz_b = v4 + a*(v5 - v4) + b*(v6 + a*(v7 - v6) - v4 - a*(v5 - v4));

		Vector3 res = yz_a + c*(yz_b - yz_a);

		morphed_verts.Push(res);
		verts[i] = res;

    }
	geomOut = TriMesh_Make(verts, faces);

}

void ModelBoxMorph::DoHarmonicDeformation(Urho3D::Vector<Vector3> deltas, Urho3D::Vector<int> ids, Variant& geomOut)
{
	//////do harmonic deformation

	//init matrices
	Eigen::MatrixXd V;
	Eigen::MatrixXi F;
	Eigen::VectorXi b;
	Eigen::MatrixXd D_bc;
	Eigen::MatrixXd D;

	//TriMeshToMatrices(mesh, V, F);
	VariantVector verts = TriMesh_GetVertexList(baseGeometry_);
	TriMeshToDoubleMatrices(baseGeometry_, V, F);


	//collect the disp vecs and indices
	int numVecs = Min(deltas.Size(), ids.Size());
	b.resize(numVecs);
	D_bc.resize(numVecs, 3);

	//create the handle and displacement vectors
	for (int i = 0; i < numVecs; i++)
	{
		int idx = ids[i];
		Vector3 v = deltas[i];
		D_bc.row(i) = Eigen::RowVector3d(v.x_, v.y_, v.z_);
		b[i] = idx;
	}

	//finally, proceed with calculation
	int power = 2;
	//igl::harmonic(V, F, b, D_bc, power, D);

	int dRows = D.rows();
	int dCols = D.cols();

	VariantVector vecsOut;

	for (int i = 0; i < D.rows(); i++)
	{
		Eigen::RowVector3d v = D.row(i);
		Vector3 dV = Vector3(v.x(), v.y(), v.z());
		vecsOut.Push(dV);

		Vector3 orgVert = verts[i].GetVector3();
		verts[i] = orgVert + dV;
	}

	//create new trimesh
	geomOut = TriMesh_Make(verts, TriMesh_GetFaceList(baseGeometry_));

	URHO3D_LOGINFO("Done Harmonic Deformation!");
}

void ModelBoxMorph::DoLinearDeformation(Urho3D::Vector<Urho3D::Vector3> deltas, Urho3D::Vector<int> ids, Urho3D::Variant & geomOut)
{
}


bool ModelBoxMorph::GetScreenRay(Urho3D::IntVector2 screenPos, Urho3D::Ray& ray)
{
	//try to get default viewport first
	Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();

	if (!activeViewport)
	{
		URHO3D_LOGINFO("No active viewport found...");
		return false;
	}

	//also check if we are casting from inside a ui region
	UIElement* uiRegion = (UIElement*)GetGlobalVar("activeUIRegion").GetPtr();


	//get default ray
	ray = activeViewport->GetScreenRay(screenPos.x_, screenPos.y_);

	if (uiRegion)
	{
		IntVector2 ePos = uiRegion->GetScreenPosition();
		IntVector2 eSize = uiRegion->GetSize();
		float x = (screenPos.x_ - ePos.x_) / (float)eSize.x_;
		float y = (screenPos.y_ - ePos.y_) / (float)eSize.y_;

		ray = activeViewport->GetCamera()->GetScreenRay(x, y);
	}

	return true;
}


bool ModelBoxMorph::Raycast()
{

	//get mouse pos via ui system to account for scaling
	UI* ui = GetSubsystem<UI>();
	IntVector2 pos = GetScaledMousePosition();

	//get general screen ray, taking ui stuff in to account
	Ray cameraRay;
	bool res = GetScreenRay(pos, cameraRay);
	if (!res)
	{
		URHO3D_LOGINFO("Bad ray or no viewport found");
		return false;
	}

	//do cast
	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, 600.0f, DRAWABLE_GEOMETRY);

	GetScene()->GetComponent<Octree>()->Raycast(query);

	if (results.Size() > 0)
	{
		for (int i = 0; i < results.Size(); i++)
		{
			BillboardSet* bs = (BillboardSet*)results[i].drawable_;
			if (bs == meshEditor_)
			{
				raycastResult_ = results[i];
				return true;
			}
		}
	}

	return false;
}
