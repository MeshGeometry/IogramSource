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
#include "CurveRenderer.h"
#include "Polyline.h"
#include "NMesh.h"
#include "TriMesh.h"

#include <Urho3D/Graphics/BillboardSet.h>

using namespace Urho3D;

String Graphics_MeshEdges::iconTexture = "Textures/Icons/Graphics_MeshEdges.png";

Graphics_MeshEdges::Graphics_MeshEdges(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
    SetName("MeshEdges");
    SetFullName("Mesh Edge Renderer");
    SetDescription("Renders mesh edges");
    
    AddInputSlot(
                 "Mesh",
                 "M",
                 "Mesh to render",
                 VAR_VARIANTMAP,
                 DataAccess::ITEM
                 );
    
    AddInputSlot(
                 "Width",
                 "W",
                 "Width of curve",
                 VAR_FLOAT,
                 DataAccess::ITEM,
                 0.001f
                 );
    
    AddInputSlot(
                 "Color",
                 "C",
                 "Color",
                 VAR_COLOR,
                 DataAccess::ITEM,
                 Color(0.2f, 0.2f, 0.2f,1.0f)
                 );
    
    AddOutputSlot(
                  "Node ID",
                  "ID",
                  "Node ID",
                  VAR_INT,
                  DataAccess::ITEM
                  );
}

void Graphics_MeshEdges::PreLocalSolve()
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

void Graphics_MeshEdges::SolveInstance(
                                           const Vector<Variant>& inSolveInstance,
                                           Vector<Variant>& outSolveInstance
                                           )
{
    
    Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    
    if (scene == NULL)
    {
        URHO3D_LOGERROR("Null scene encountered.");
        return;
    }
    
    Node* node = scene->CreateChild("CurvePreview");
    
    Material* mat = NULL;
    mat = cache->GetResource<Material>("Materials/BasicCurve.xml");
    
    if (!mat)
    {
        cache->GetResource<Material>("Materials/BasicWebAlpha.xml");
    }
    
    bool isTriMesh = TriMesh_Verify(inSolveInstance[0]);
    bool isNMesh = NMesh_Verify(inSolveInstance[0]);
    
    if (!mat)
    {
        SetAllOutputsNull(outSolveInstance);
        return;
    }
    if (!(isTriMesh || isNMesh))
    {
        SetAllOutputsNull(outSolveInstance);
        return;
    }
    
    
    Vector<Variant> face_polylines;
    // compute mesh face polylines
    if (isNMesh){
        face_polylines = NMesh_ComputeWireframePolylines(inSolveInstance[0]);
    }
    
    if (isTriMesh){
        Variant trimesh = inSolveInstance[0];
        VariantVector vertex_list = TriMesh_GetVertexList(trimesh);
        VariantVector face_list = TriMesh_GetFaceList(trimesh);
        
//        int numFaces = face_list.Size()/3;
//        
//        for (unsigned i = 0; i < numFaces; ++i) {
//            VariantVector curFace;
//            int a = face_list[3 * i].GetInt();
//            int b = face_list[3 * i + 1].GetInt();
//            int c = face_list[3 * i + 2].GetInt();
//            
//            curFace.Push(a);
//            curFace.Push(c);
//            curFace.Push(b);
//            
//            VariantVector poly_vertex_list;
//            poly_vertex_list.Push(vertex_list[a].GetVector3());
//            poly_vertex_list.Push(vertex_list[b].GetVector3());
//            poly_vertex_list.Push(vertex_list[c].GetVector3());
//            
//            Variant cur_poly = Polyline_Make(poly_vertex_list);
//            face_polylines.Push(cur_poly);
//        
//        
//        
//        }
        
        // rendering edges instead -- does it make a difference?
        Urho3D::Vector<Urho3D::Pair<int,int>> edge_list = TriMesh_ComputeEdges(trimesh);
        for (unsigned i = 0; i < edge_list.Size(); ++i){
                        VariantVector poly_vertex_list;
                        poly_vertex_list.Push(vertex_list[edge_list[i].first_].GetVector3());
                        poly_vertex_list.Push(vertex_list[edge_list[i].second_].GetVector3());
            
                        Variant cur_poly = Polyline_Make(poly_vertex_list);
                        face_polylines.Push(cur_poly);
            
        }
        
    }
    
    
    for (int i = 0; i < face_polylines.Size(); ++i)
    {
        VariantMap curPolyline = face_polylines[i].GetVariantMap();
        //VariantVector verts = curPolyline["vertices"].GetVariantVector();
        VariantVector verts = Polyline_ComputeSequentialVertexList(curPolyline);
        
        if (verts.Size() < 2)
        {
            SetAllOutputsNull(outSolveInstance);
            return;
        }
        
        BillboardSet* curveDisplay = node->CreateComponent<BillboardSet>();
        curveDisplay->SetNumBillboards(verts.Size());
        curveDisplay->SetMaterial(mat);
        curveDisplay->SetSorted(true);
        curveDisplay->SetFaceCameraMode(FaceCameraMode::FC_DIRECTION);
        //curveDisplay->SetFixedScreenSize(true);
        
        SharedPtr<Material> clonedMat = mat->Clone();
        clonedMat->SetPixelShaderDefines("DRAWPOINT");
        BillboardSet* controlPointDisplay = node->CreateComponent<BillboardSet>();
        controlPointDisplay->SetNumBillboards(verts.Size());
        controlPointDisplay->SetMaterial(clonedMat);
        controlPointDisplay->SetSorted(true);
        controlPointDisplay->SetFaceCameraMode(FaceCameraMode::FC_ROTATE_XYZ);
        //controlPointDisplay->SetFixedScreenSize(true);
        
        float width = inSolveInstance[1].GetFloat();
        Color col = inSolveInstance[2].GetColor();
        
        mat->SetShaderParameter("MatDiffColor", col);
        clonedMat->SetShaderParameter("MatDiffColor", col);
        
        for (int i = 0; i < verts.Size() - 1; i++)
        {
            //render the edge
            Billboard* bb = curveDisplay->GetBillboard(i);
            
            Vector3 vA = verts[i].GetVector3();
            Vector3 vB = verts[i + 1].GetVector3();
            Vector3 edgeVec = vB - vA;
            Vector3 midPt = vA + 0.5f * edgeVec;
            
            bb->position_ = midPt;
            bb->size_ = Vector2(width, 0.5f * (edgeVec.Length()));
            bb->direction_ = edgeVec;
            bb->enabled_ = true;
            bb->color_ = col;
            
            //render the point
            bb = controlPointDisplay->GetBillboard(i);
            bb->position_ = vA;
            bb->size_ =  0.59f * Vector2(width, width);
            bb->enabled_ = true;
            bb->color_ = col;
            
        }
        
        curveDisplay->Commit();

    }

    trackedItems.Push(node->GetID());
    outSolveInstance[0] = node->GetID();
}
