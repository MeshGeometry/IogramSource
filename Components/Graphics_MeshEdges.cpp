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


#include "Graphics_MeshEdges.h"
#include "TriMesh.h"
#include "NMesh.h"
#include "Polyline.h"

#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Graphics_MeshEdges::iconTexture = "Textures/Icons/Scene_MeshRenderer.png";

namespace {
	//vertex data types
	struct VertexData
	{
		Urho3D::Vector3 position; // size of 12 bytes +
		Urho3D::Vector3 normal; // size of 12 bytes +
		unsigned color;
	};
}

Graphics_MeshEdges::Graphics_MeshEdges(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("MeshRenderer");
	SetFullName("MeshRenderer");
	SetDescription("Converts a mesh to a viewable object in the scene.");

	AddInputSlot(
		"Mesh",
		"M",
		"Mesh structure",
		VAR_VARIANTMAP,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Color",
		"C",
		"Color",
		VAR_COLOR,
		DataAccess::ITEM,
		Color::BLACK
	);

	AddInputSlot(
		"Width",
		"W",
		"Line width",
		VAR_FLOAT,
		DataAccess::ITEM,
		0.1f
	);

	AddOutputSlot(
		"NodeID",
		"ID",
		"Node ID",
		VAR_INT,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Model",
		"Ptr",
		"Static Model Pointer",
		VAR_PTR,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"ModelName",
		"ModelName",
		"ModelName",
		VAR_STRING,
		DataAccess::ITEM
	);

}

Graphics_MeshEdges::~Graphics_MeshEdges()
{

}

void Graphics_MeshEdges::PreLocalSolve()
{
	//delete old nodes
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedItems.Size(); i++)
		{
			Node* oldNode = scene->GetNode(trackedItems[i]);
			if (oldNode)
			{
				oldNode->Remove();
			}

		}

		trackedItems.Clear();
	}

	//release resource
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	for (int i = 0; i < trackedResources.Size(); i++)
	{
		String resourcePath = trackedResources[i];
		if (resourcePath.Contains("tmp/models/"))
			rc->ReleaseResource<Model>(trackedResources[i]);
		if (resourcePath.Contains("tmp/materials/"))
			rc->ReleaseResource<Material>(trackedResources[i]);
	}

}

void Graphics_MeshEdges::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	//some checks
	if (inSolveInstance[0].GetType() != VAR_VARIANTMAP)
	{
		URHO3D_LOGERROR("Expected a VariantMap and was given a " + inSolveInstance[0].GetTypeName());
		return;
	}

	VariantMap mData = inSolveInstance[0].GetVariantMap();

	bool isTriMesh = TriMesh_Verify(inSolveInstance[0]);
	bool isNMesh = NMesh_Verify(inSolveInstance[0]);

	Context* context = GetContext();

	if (isTriMesh || isNMesh)
	{
		Color col = inSolveInstance[1].GetColor();
		float width = inSolveInstance[2].GetFloat();


		// TRIMESH render
		if (isTriMesh) {
			Variant model_pointer;
			String model_name;
			int nodeId = TriMesh_Render(inSolveInstance[0], context, width, col, model_pointer, model_name);
			if (nodeId == -1)
				SetAllOutputsNull(outSolveInstance);

			trackedItems.Push(nodeId);
			outSolveInstance[0] = nodeId;
			outSolveInstance[1] = model_pointer.GetPtr();
			outSolveInstance[2] = model_name;
		}//TRIMESH render

		 // NMESH render
		if (isNMesh) {
			Variant model_pointer;
			String model_name;
			int nodeId = NMesh_Render(inSolveInstance[0], context, width, col, model_pointer, model_name);
			if (nodeId == -1)
				SetAllOutputsNull(outSolveInstance);

			trackedItems.Push(nodeId);
			outSolveInstance[0] = nodeId;
			outSolveInstance[1] = model_pointer.GetPtr();
			outSolveInstance[2] = model_name;
		}//NMESH render
	}
	else
	{
		URHO3D_LOGERROR("Incomplete data given in Mesh paramater. Meshes require vertices, faces, and normals.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
}

int Graphics_MeshEdges::TriMesh_Render(Urho3D::Variant trimesh,
	Urho3D::Context* context,
	float lineWidth,
	Urho3D::Color mainColor,
	Urho3D::Variant& model_pointer,
	Urho3D::String& model_name)
{

	Vector<VertexData> vbd;
	Vector<Vector3> tmpVerts;
	Vector<int> tmpFaces;

	VariantVector verts = TriMesh_GetVertexList(trimesh);
	VariantVector faces = TriMesh_GetFaceList(trimesh);
	VariantVector normals;


	normals = TriMesh_ComputeFaceNormals(trimesh, true);
	tmpFaces.Resize(faces.Size());
	tmpVerts.Resize(faces.Size());
	vbd.Resize(faces.Size());

	unsigned int vCol = Urho3D::Color::WHITE.ToUInt();

	//these are what we will use to do barycentric coords in shader
	Color vCols[] = {
		Color::RED,
		Color::GREEN,
		Color::BLUE
	};

	//render with duplicate verts for flat face shading
	int triCounter = 0;
	for (unsigned i = 0; i < faces.Size(); i++)
	{
		int fId = faces[i].GetInt();
		int normId = (int)floor((float)i / 3.0f);
		//unsigned int col = vCols[normId%numColors].GetColor().ToUInt();
		if (fId < (int)verts.Size())
		{
			vbd[i].position = verts[fId].GetVector3();
			vbd[i].normal = normals[normId].GetVector3();
			vbd[i].color = vCols[i % 3].ToUInt();
			tmpVerts[i] = vbd[i].position;
		}

		tmpFaces[i] = i;

	}

	SharedPtr<VertexBuffer> vb(new VertexBuffer(context));
	SharedPtr<IndexBuffer> ib(new IndexBuffer(context));

	// Shadowed buffer needed for raycasts to work, and so that data can be automatically restored on device loss
	vb->SetShadowed(true);
	vb->SetSize(vbd.Size(), Urho3D::MASK_POSITION | Urho3D::MASK_NORMAL | Urho3D::MASK_COLOR);
	vb->SetData((void*)&vbd[0]);

	ib->SetShadowed(true);
	ib->SetSize(tmpFaces.Size(), true);
	ib->SetData(&tmpFaces[0]);

	Geometry* geom = new Geometry(context);
	geom->SetNumVertexBuffers(1);
	geom->SetVertexBuffer(0, vb);
	geom->SetIndexBuffer(ib);
	geom->SetDrawRange(Urho3D::TRIANGLE_LIST, 0, faces.Size());

	SharedPtr<Model> model(new Model(context));
	model->SetNumGeometries(1);
	model->SetGeometry(0, 0, geom);
	model->SetBoundingBox(BoundingBox(&tmpVerts[0], tmpVerts.Size()));
	model->SetGeometryCenter(0, Vector3::ZERO);

	Vector<SharedPtr<VertexBuffer>> allVBuffers;
	Vector<SharedPtr<IndexBuffer>> allIBuffers;

	allVBuffers.Push(vb);
	allIBuffers.Push(ib);

	PODVector<unsigned int> morphStarts;
	PODVector<unsigned int> morphRanges;

	model->SetVertexBuffers(allVBuffers, morphStarts, morphRanges);
	model->SetIndexBuffers(allIBuffers);

	//create a new node
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	Material* mat = GetSubsystem<ResourceCache>()->GetResource<Material>(normalMat);
	if (!scene || !mat)
	{
		//SetAllOutputsNull(outSolveInstance);
		return -1;
	}

	Node* mNode = scene->CreateChild("MeshPreviewNode");
	StaticModel* sm = mNode->CreateComponent<StaticModel>();
	int smID = sm->GetID();

	SharedPtr<Material> cloneMat = mat->Clone();
	cloneMat->SetName("tmp/materials/generated_mat_" + String(smID));
	cloneMat->SetShaderParameter("MatDiffColor", mainColor);
	cloneMat->SetShaderParameter("LineWidth", lineWidth);
	model->SetName("tmp/models/generated_model_" + String(smID));

	//add these to the resource cache
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	bool res = rc->AddManualResource(model);
	res = rc->AddManualResource(cloneMat);
	trackedResources.Push(cloneMat->GetName());
	trackedResources.Push(model->GetName());

	sm->SetModel(model);
	sm->SetMaterial(cloneMat);

	model_pointer = Variant(sm);
	model_name = model->GetName();

	return mNode->GetID();
}

int Graphics_MeshEdges::NMesh_Render(Urho3D::Variant nMesh,
	Urho3D::Context* context,
	float lineWidth,
	Urho3D::Color mainColor,
	Urho3D::Variant& model_pointer,
	Urho3D::String& model_name)
{

	Vector<VertexData> vbd;
	Vector<Vector3> tmpVerts;
	Vector<int> tmpFaces;

	VariantVector normals;
	VariantVector ngonTriList;
	VariantVector ngonTriList_unified;

	//Urho3D::Variant convertedMesh = NMesh_ConvertToTriMesh_P2T(nMesh, ngonTriList);

	Urho3D::Variant convertedMesh = NMesh_ConvertToTriMesh(nMesh, ngonTriList);

	Urho3D::Variant unifiedMesh = TriMesh_UnifyNormals(convertedMesh);

	VariantVector verts = TriMesh_GetVertexList(unifiedMesh);

	// only for the size:
	VariantVector triFaceList = TriMesh_GetFaceList(unifiedMesh);
	int numb_tris = triFaceList.Size();

	// make a copy of faceTris that has the new ordering
	int counter = 0;
	for (int i = 0; i < ngonTriList.Size(); ++i) {
		VariantVector currNgonTris = ngonTriList[i].GetVariantVector();
		VariantVector currNgonTris_unified;
		for (int j = 0; j < currNgonTris.Size(); ++j) {
			currNgonTris_unified.Push(Variant(triFaceList[counter].GetInt()));
			counter++;
		}
		ngonTriList_unified.Push(Variant(currNgonTris_unified));
	}


	normals = TriMesh_ComputeFaceNormals(unifiedMesh, true);
	tmpFaces.Resize(numb_tris);
	tmpVerts.Resize(numb_tris);
	vbd.Resize(numb_tris);

	unsigned int vCol = Urho3D::Color::WHITE.ToUInt();

	//render with duplicate verts for flat face shading
	// assign every triangle from a single ngon face to one normal.
	int faceCounter = 0;
	for (unsigned i = 0; i < ngonTriList_unified.Size(); i++)
	{
		// Get the face list for the first n-gon face
		VariantVector faces = ngonTriList_unified[i].GetVariantVector();
		// take the first face of the ngon to compute the norm of the ngon
		int normId = faceCounter;
		for (int j = 0; j < faces.Size(); ++j) {
			int ID = 3 * faceCounter + j;
			int fId = faces[j].GetInt();
			if (fId < (int)verts.Size())
			{
				vbd[ID].position = verts[fId].GetVector3();
				vbd[ID].normal = normals[normId].GetVector3();
				vbd[ID].color = vCol;
				tmpVerts[ID] = vbd[ID].position;
			}
			tmpFaces[ID] = ID;
		}
		faceCounter += faces.Size() / 3;
	}


	SharedPtr<VertexBuffer> vb(new VertexBuffer(context));
	SharedPtr<IndexBuffer> ib(new IndexBuffer(context));

	// Shadowed buffer needed for raycasts to work, and so that data can be automatically restored on device loss
	vb->SetShadowed(true);
	vb->SetSize(vbd.Size(), Urho3D::MASK_POSITION | Urho3D::MASK_NORMAL | Urho3D::MASK_COLOR);
	vb->SetData((void*)&vbd[0]);

	ib->SetShadowed(true);
	ib->SetSize(tmpFaces.Size(), true);
	ib->SetData(&tmpFaces[0]);

	Geometry* geom = new Geometry(context);
	geom->SetNumVertexBuffers(1);
	geom->SetVertexBuffer(0, vb);
	geom->SetIndexBuffer(ib);
	geom->SetDrawRange(Urho3D::TRIANGLE_LIST, 0, numb_tris);

	SharedPtr<Model> model(new Model(context));
	model->SetNumGeometries(1);
	model->SetGeometry(0, 0, geom);
	model->SetBoundingBox(BoundingBox(&tmpVerts[0], tmpVerts.Size()));
	model->SetGeometryCenter(0, Vector3::ZERO);

	Vector<SharedPtr<VertexBuffer>> allVBuffers;
	Vector<SharedPtr<IndexBuffer>> allIBuffers;

	allVBuffers.Push(vb);
	allIBuffers.Push(ib);

	PODVector<unsigned int> morphStarts;
	PODVector<unsigned int> morphRanges;

	model->SetVertexBuffers(allVBuffers, morphStarts, morphRanges);
	model->SetIndexBuffers(allIBuffers);

	//create a new node
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	Material* mat = GetSubsystem<ResourceCache>()->GetResource<Material>(normalMat);
	if (!scene || !mat)
	{
		//SetAllOutputsNull(outSolveInstance);
		return -1;
	}

	Node* mNode = scene->CreateChild("MeshPreviewNode");
	StaticModel* sm = mNode->CreateComponent<StaticModel>();
	int smID = sm->GetID();

	SharedPtr<Material> cloneMat = mat->Clone();
	cloneMat->SetName("tmp/materials/generated_mat_" + String(smID));
	cloneMat->SetShaderParameter("MatDiffColor", mainColor);
	model->SetName("tmp/models/generated_model_" + String(smID));

	//add these to the resource cache
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	bool res = rc->AddManualResource(model);
	res = rc->AddManualResource(cloneMat);
	trackedResources.Push(cloneMat->GetName());
	trackedResources.Push(model->GetName());

	sm->SetModel(model);
	sm->SetMaterial(cloneMat);
	//sm->SetMaterial(mat);

	model_pointer = Variant(sm);
	model_name = model->GetName();

	return mNode->GetID();
}


