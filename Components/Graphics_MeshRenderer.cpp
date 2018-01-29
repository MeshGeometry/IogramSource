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

#include <iostream>

#include "Graphics_MeshRenderer.h"
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

String Graphics_MeshRenderer::iconTexture = "Textures/Icons/Scene_MeshRenderer.png";

Graphics_MeshRenderer::Graphics_MeshRenderer(Urho3D::Context* context) : IoComponentBase(context, 4, 3)
{
	SetName("MeshRenderer");
	SetFullName("MeshRenderer");
	SetDescription("Converts a mesh to a viewable object in the scene.");;

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh Structure");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Material");
	inputSlots_[1]->SetVariableName("MT");
	inputSlots_[1]->SetDescription("Path to material.");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[2]->SetName("SplitVertices");
	inputSlots_[2]->SetVariableName("S");
	inputSlots_[2]->SetDescription("Split the vertices for flat shading");
	inputSlots_[2]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(true);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("Color");
	inputSlots_[3]->SetVariableName("C");
	inputSlots_[3]->SetDescription("MainColor");
	inputSlots_[3]->SetVariantType(VariantType::VAR_COLOR);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(Color::WHITE);
	inputSlots_[3]->DefaultSet();

	outputSlots_[0]->SetName("NodeID");
	outputSlots_[0]->SetVariableName("ID");
	outputSlots_[0]->SetDescription("ID of rendered node");
	outputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("StaticModelPointer");
	outputSlots_[1]->SetVariableName("SM");
	outputSlots_[1]->SetDescription("Void Pointer to Static Model");
	outputSlots_[1]->SetVariantType(VariantType::VAR_PTR);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[2]->SetName("ModelName");
	outputSlots_[2]->SetVariableName("ModelName");
	outputSlots_[2]->SetDescription("ModelName");
	outputSlots_[2]->SetVariantType(VariantType::VAR_STRING);
	outputSlots_[2]->SetDataAccess(DataAccess::ITEM);
}

Graphics_MeshRenderer::~Graphics_MeshRenderer()
{

}

void Graphics_MeshRenderer::PreLocalSolve()
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
		if(resourcePath.Contains("tmp/models/"))
			rc->ReleaseResource<Model>(trackedResources[i]);
		if (resourcePath.Contains("tmp/materials/"))
			rc->ReleaseResource<Material>(trackedResources[i]);
	}

}

void Graphics_MeshRenderer::SolveInstance(
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
		Color col = inSolveInstance[3].GetColor();
		String matPath = inSolveInstance[1].GetString();
		bool split = inSolveInstance[2].GetBool();

		if (inSolveInstance[1].GetType() == VAR_PTR)
		{
			Material* inMat = (Material*)inSolveInstance[1].GetPtr();
			if (inMat)
			{
				matPath = inMat->GetName();
			}
		}

		//adjust defaults so that alpha behaves correctly
		if (col.a_ < 1.0f && matPath.Empty())
		{
			matPath = normalAlphaMat;
		}
		else if (col.a_ >= 1.0f && matPath.Empty())
		{
			matPath = normalMat;
		}
        
        // TRIMESH render
        if (isTriMesh){
            Variant model_pointer;
			String model_name;
            int nodeId = TriMesh_Render(inSolveInstance[0], context, matPath, split, col, model_pointer, model_name);
            if (nodeId == -1)
                SetAllOutputsNull(outSolveInstance);
        
            trackedItems.Push(nodeId);
            outSolveInstance[0] = nodeId;
            outSolveInstance[1] = model_pointer.GetPtr();
			outSolveInstance[2] = model_name;
        }//TRIMESH render
        
        // NMESH render
        if (isNMesh){
            Variant model_pointer;
	    String model_name;
            int nodeId = NMesh_Render(inSolveInstance[0], context, matPath, split, col, model_pointer, model_name);
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

int Graphics_MeshRenderer::TriMesh_Render(Urho3D::Variant trimesh,
                                          Urho3D::Context* context,
                                          Urho3D::String material_path,
                                          bool flatShaded,
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
    
    if (flatShaded)
    {
        normals = TriMesh_ComputeFaceNormals(trimesh, true);
        tmpFaces.Resize(faces.Size());
        tmpVerts.Resize(faces.Size());
        vbd.Resize(faces.Size());
        
        unsigned int vCol = Urho3D::Color::WHITE.ToUInt();
        
        //render with duplicate verts for flat face shading
        for (unsigned i = 0; i < faces.Size(); i++)
        {
            int fId = faces[i].GetInt();
            int normId = (int)floor((float)i / 3.0f);
			Vector3 n = normals[normId].GetVector3();
			Color vCol = Color(n.x_, n.y_, n.z_, 1.0f);
			vCol = 0.5f * (vCol + Color::WHITE);
            //unsigned int col = vCols[normId%numColors].GetColor().ToUInt();
            if (fId < (int)verts.Size())
            {
                vbd[i].position = verts[fId].GetVector3();
				vbd[i].normal = n;
				vbd[i].color = vCol.ToUInt();
                tmpVerts[i] = vbd[i].position;
            }
            
            tmpFaces[i] = i;
            
        }
    }
    else
    {
        normals = TriMesh_ComputeVertexNormals(trimesh, true);
        tmpFaces.Resize(faces.Size());
        tmpVerts.Resize(verts.Size());
        vbd.Resize(verts.Size());
        unsigned int vCol = Color::WHITE.ToUInt();
        
        for (unsigned i = 0; i < verts.Size(); i++)
        {
			Vector3 n = normals[i].GetVector3();
			Color vCol = Color(n.x_, n.y_, n.z_, 1.0f);
			vCol = 0.5f * (vCol + Color::WHITE);
			
			vbd[i].position = verts[i].GetVector3();
			vbd[i].normal = n;
			vbd[i].color = vCol.ToUInt();
            tmpVerts[i] = verts[i].GetVector3();
        }
        
        for (unsigned i = 0; i < faces.Size(); i++)
        {
            tmpFaces[i] = faces[i].GetInt();
        }
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
    Material* mat = GetSubsystem<ResourceCache>()->GetResource<Material>(material_path);
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
	Color specColor = cloneMat->GetShaderParameter("MatSpecColor").GetColor();
	Color existingColor = cloneMat->GetShaderParameter("MatDiffColor").GetColor();
	Color blendColor = existingColor.MultiplyComponents(mainColor);
    cloneMat->SetShaderParameter("MatDiffColor", blendColor);
    model->SetName("tmp/models/generated_model_" + String(smID));
    
    //add these to the resource cache
    ResourceCache* rc = GetSubsystem<ResourceCache>();
    bool res = rc->AddManualResource(model);
    res = rc->AddManualResource(cloneMat);
    trackedResources.Push(cloneMat->GetName());
    trackedResources.Push(model->GetName());
    
    sm->SetModel(model);
    //sm->SetMaterial(cloneMat);
    sm->SetMaterial(cloneMat);
	sm->SetCastShadows(true);
	sm->SetShadowDistance(100.0f);
    
    model_pointer = Variant(sm);
    
	std::cout << "model->GetName().CString() = " << model->GetName().CString() << std::endl;
	model_name = model->GetName();

    return mNode->GetID();
}

int Graphics_MeshRenderer::NMesh_Render(Urho3D::Variant nMesh,
                                          Urho3D::Context* context,
                                          Urho3D::String material_path,
                                          bool flatShaded,
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
    int test =  ngonTriList.Size();
    
    Urho3D::Variant unifiedMesh = TriMesh_UnifyNormals(convertedMesh);
    
    VariantVector verts = TriMesh_GetVertexList(unifiedMesh);
    
    // only for the size:
    VariantVector triFaceList = TriMesh_GetFaceList(unifiedMesh);
    int numb_tris = triFaceList.Size();
    
    // make a copy of faceTris that has the new ordering
    int counter = 0;
    for (int i = 0; i < ngonTriList.Size(); ++i){
        VariantVector currNgonTris = ngonTriList[i].GetVariantVector();
        VariantVector currNgonTris_unified;
        for (int j = 0; j < currNgonTris.Size(); ++j) {
            currNgonTris_unified.Push(Variant(triFaceList[counter].GetInt()));
            counter++;
        }
        ngonTriList_unified.Push(Variant(currNgonTris_unified));
    }
    
    // for now render each nGon face as actually planar
    if (flatShaded)
    {
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
            for (int j = 0; j < faces.Size(); ++j){
                int ID = 3*faceCounter +j;
                int fId = faces[j].GetInt();
				Vector3 n = normals[normId].GetVector3();;
				Color vCol = Color(n.x_, n.y_, n.z_, 1.0f);
				vCol = 0.5f * (vCol + Color::WHITE);
                if (fId < (int)verts.Size())
                {
                    vbd[ID].position = verts[fId].GetVector3();
					vbd[ID].normal = n;
                    vbd[ID].color = vCol.ToUInt();
                    tmpVerts[ID] = vbd[ID].position;
                }
                tmpFaces[ID] = ID;
            }
            faceCounter += faces.Size()/3;
        }
    }
    else
    {
        // if it is smooth-shaded, just compute as smoothshaded trimesh
		String path_to_resource_ref;
        return TriMesh_Render(unifiedMesh, context, material_path, flatShaded, mainColor, model_pointer, path_to_resource_ref);
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
    Material* mat = GetSubsystem<ResourceCache>()->GetResource<Material>(material_path);
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
	Color existingColor = cloneMat->GetShaderParameter("MatDiffColor").GetColor();
	Color blendColor = existingColor.MultiplyComponents(mainColor);
	cloneMat->SetShaderParameter("MatDiffColor", blendColor);
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


