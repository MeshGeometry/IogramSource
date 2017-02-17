#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Graphics/Model.h>


// vertex_list is a list of Vector3's
// face_list has a list of integers with the following format: 
// entry 0 : number of indices in first face, say = 4
// entries 1, 2, 3, 4 : indices of first face
// entry 5 : number of indices in second face
// etc.

Urho3D::Variant NMesh_Make(const Urho3D::VariantVector& vertex_list, const Urho3D::VariantVector& face_list);

bool NMesh_Verify(const Urho3D::Variant& nmesh);

Urho3D::VariantVector NMesh_GetVertexList(const Urho3D::Variant& nmesh);
Urho3D::VariantVector NMesh_GetFaceList(const Urho3D::Variant& nmesh);

Urho3D::Variant NMesh_ConvertToTriMesh(const Urho3D::Variant& nmesh);
Urho3D::Variant NMesh_ConvertToTriMesh_P2T(const Urho3D::Variant& nmesh, Urho3D::VariantVector& faceTris);

Urho3D::Vector<Urho3D::Variant> NMesh_ComputeWireframePolylines(const Urho3D::Variant& nmesh);

Urho3D::Variant NMesh_GetFacePolyline(const Urho3D::Variant& nmesh,
	int face_ID,
	Urho3D::VariantVector& face_indices);

//Display functions
Urho3D::Model* NMesh_GetRenderMesh(const Urho3D::Variant& nMesh, Urho3D::Context* context, Urho3D::VariantVector vColors, bool split=false);
