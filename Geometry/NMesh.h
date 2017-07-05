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


#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/AngelScript/APITemplates.h>

// vertex_list is a list of Vector3's
// face_list has a list of integers with the following format: 
// entry 0 : number of indices in first face, say = 4
// entries 1, 2, 3, 4 : indices of first face
// entry 5 : number of indices in second face
// etc.

Urho3D::Variant NMesh_Make(const Urho3D::VariantVector& vertex_list, const Urho3D::VariantVector& face_list); // REGISTERED as NMesh_MakeFromVariantArrays

bool NMesh_Verify(const Urho3D::Variant& nmesh); // REGISTERED

Urho3D::VariantVector NMesh_GetVertexList(const Urho3D::Variant& nmesh); // REGISTERED as NMesh_GetVertexArray
Urho3D::VariantVector NMesh_GetFaceList(const Urho3D::Variant& nmesh); // REGISTERED as NMesh_GetFaceArray

Urho3D::Variant NMesh_ConvertToTriMesh(const Urho3D::Variant& nmesh); // REGISTERED as NMesh_ConvertToTriMeshFromVariant
Urho3D::Variant NMesh_ConvertToTriMesh(const Urho3D::Variant& nmesh, Urho3D::VariantVector& faceTris); // REGISTERED as NMesh_ConvertToTriMeshFromVariantAndFaceTris

Urho3D::Variant NMesh_ConvertToTriMesh_P2T(const Urho3D::Variant& nmesh, Urho3D::VariantVector& faceTris);

Urho3D::Vector<Urho3D::Variant> NMesh_ComputeWireframePolylines(const Urho3D::Variant& nmesh); // REGISTERED as NMesh_ComputeWireframePolylinesArray

Urho3D::Variant NMesh_GetFacePolyline(const Urho3D::Variant& nmesh,
	int face_ID,
	Urho3D::VariantVector& face_indices); // REGISTERED as NMesh_GetFacePolylineFromIndicesArray

Urho3D::Vector<Urho3D::Pair<int, int>> NMesh_ComputeEdges(const Urho3D::Variant& NMesh);

//Display functions
Urho3D::Model* NMesh_GetRenderMesh(const Urho3D::Variant& nMesh, Urho3D::Context* context, Urho3D::VariantVector vColors, bool split=false);

// for scripts
Urho3D::Variant NMesh_MakeFromVariantArrays(Urho3D::CScriptArray* vertex_array, Urho3D::CScriptArray* face_array);
Urho3D::CScriptArray* NMesh_GetVertexArray(const Urho3D::Variant& nmesh);
Urho3D::CScriptArray* NMesh_GetFaceArray(const Urho3D::Variant& nmesh);
Urho3D::Variant NMesh_ConvertToTriMeshFromVariant(const Urho3D::Variant& nmesh);
Urho3D::Variant NMesh_ConvertToTriMeshFromVariantAndFaceTris(const Urho3D::Variant& nmesh, Urho3D::CScriptArray* face_tris_array);
Urho3D::CScriptArray* NMesh_ComputeWireframePolylinesArray(const Urho3D::Variant& nmesh);
Urho3D::Variant NMesh_GetFacePolylineFromIndicesArray(const Urho3D::Variant& nmesh, int face_ID, Urho3D::CScriptArray* face_indices_arr);

bool RegisterNMeshFunctions(Urho3D::Context* context);
